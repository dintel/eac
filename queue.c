#include <stdbool.h>
#include "queue.h"
#include "lz77.h"

void *queue_job_thread_encode(void *param);

queue_t *queue_init()
{
    queue_t *q = malloc(sizeof(queue_t));
    q->first = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_mutex_init(&q->cv_mutex, NULL);
    pthread_cond_init(&q->cv, NULL);
    return q;
}

void queue_destroy(queue_t *q)
{
    job_t *tmp, *job = q->first;
    while(job != NULL) {
        tmp = job->next;
        free(job);
        job = tmp;
    }
    pthread_mutex_destroy(&q->mutex);
    pthread_mutex_destroy(&q->cv_mutex);
    pthread_cond_destroy(&q->cv);
    free(q);
}

job_t *queue_run_job(queue_t *q)
{
    pthread_mutex_lock(&q->mutex);
    job_t *job = q->first;
    int result;
    while(job != NULL && job->status != JOB_STATUS_NEW) {
        job = job->next;
    }
    if(job != NULL) {
        job->status = JOB_STATUS_RUNNING;
        result = pthread_create(&job->thread,NULL,queue_job_thread_encode,(void *)job);
        if(result != 0) {
            printf("Failed spawning thread!\n");
            exit(EXIT_FAILURE);
        }
    }
    pthread_mutex_unlock(&q->mutex);
    return job;
}

void queue_add_job(queue_t *q, block_t *block,size_t window_size)
{
    pthread_mutex_lock(&q->mutex);
    job_t *job = malloc(sizeof(job_t));
    if(job == NULL) {
        printf("Failed allocating memory for new job!\n");
        exit(EXIT_FAILURE);
    }
    job->status = JOB_STATUS_NEW;
    job->block = block;
    job->window_size = window_size;
    job->queue = q;
    job->next = q->first;
    q->first = job;
    pthread_mutex_unlock(&q->mutex);
}

job_t *queue_fetch_finished(queue_t *q)
{
    pthread_mutex_lock(&q->mutex);
    job_t *job = NULL;
    if(q->first != NULL) {
        if(q->first->status == JOB_STATUS_DONE) {
            job = q->first;
            q->first = q->first->next;
        } else if(q->first->next != NULL) {
            job_t *ptr = q->first->next, *prev = q->first;
            while(ptr != NULL) {
                if(ptr->status == JOB_STATUS_DONE) {
                    prev->next = ptr->next;
                    job = ptr;
                    break;
                }
                ptr = ptr->next;
                prev = prev->next;
            }
        }
    }
    if(job != NULL) {
        job->next = NULL;
    }
    pthread_mutex_unlock(&q->mutex);
    return job;
}

int queue_is_finished(queue_t *q)
{
    job_t *cur = q->first;
    while(cur != NULL) {
        if(cur->status != JOB_STATUS_DONE) {
            return false;
        }
        cur = cur->next;
    }
    return true;
}

void queue_destroy_job(job_t *job)
{
    free(job);
}

void queue_run(queue_t *queue,int threads)
{
    pthread_mutex_lock(&queue->cv_mutex);
    for(int i=0; i < threads; i++) {
        queue_run_job(queue);
    }
    while(!queue_is_finished(queue)) {
        pthread_cond_wait(&queue->cv,&queue->cv_mutex);
        queue_run_job(queue);
    }
    pthread_mutex_unlock(&queue->cv_mutex);
}

void *queue_job_thread_encode(void *param)
{
    job_t *job = (job_t *)param;
    bit_string_t *result;
    if(job->block->prev_block != NULL)
        result = lz77_encode(job->block->block,job->window_size,job->block->prev_block->block);
    else
        result = lz77_encode(job->block->block,job->window_size,NULL);
    block_update(job->block,job->window_size,result);
    bit_string_destroy(result);
    job->status = JOB_STATUS_DONE;
    pthread_mutex_lock(&job->queue->cv_mutex);
    pthread_cond_signal(&job->queue->cv);
    pthread_mutex_unlock(&job->queue->cv_mutex);
    pthread_exit(NULL);
}
