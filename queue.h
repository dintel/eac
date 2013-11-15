#ifndef QUEUE_H
#define QUEUE_H

#include "bit_string.h"
#include <pthread.h>
#include <stdint.h>
#include "block.h"

typedef enum {
    JOB_STATUS_NEW,
    JOB_STATUS_RUNNING,
    JOB_STATUS_DONE
} JOB_STATUS;

typedef struct job_t {
    JOB_STATUS status;
    block_t *block;
    size_t window_size;
    pthread_t thread;
    struct queue_t *queue;
    struct job_t *next;
} job_t;

typedef struct queue_t {
    job_t *first;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    pthread_mutex_t cv_mutex;
} queue_t;

queue_t *queue_init();
void queue_destroy(queue_t *q);

job_t *queue_run_job(queue_t *q);
void queue_add_job(queue_t *q, block_t *block,size_t window_size);
job_t *queue_fetch_finished(queue_t *q);
void queue_run(queue_t *queue,int threads);

void queue_destroy_job(job_t *job);

#endif
