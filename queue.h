#ifndef QUEUE_H
#define QUEUE_H

#include "bit_string.h"
#include <pthread.h>
#include <stdint.h>
#include "block.h"

/** \addtogroup queue
 * @{
 */

/**
 * \brief job status
 *
 * Enumeration of all possible job statuses.
 */
typedef enum {
    JOB_STATUS_NEW,             /**< New job that was not started yet  */
    JOB_STATUS_RUNNING,         /**< Job that is currently running */
    JOB_STATUS_DONE             /**< Finished job  */
} JOB_STATUS;

/**
 * \brief job structure
 *
 * This type holds all information that is related to single job.
 */
typedef struct job_t {
    JOB_STATUS status;          /**< job status  */
    block_t *block;             /**< block on which job is run  */
    size_t window_size;         /**< window size that must be checked */
    pthread_t thread;           /**< job's thread */
    struct queue_t *queue;      /**< queue of job  */
    struct job_t *next;         /**< pointer to next job */
} job_t;

/**
 * \brief queue of jobs
 *
 * This type holds information about queue of all jobs.
 */
typedef struct queue_t {
    job_t *first;               /**< pointer to start of linked list of jobs */
    pthread_mutex_t mutex;      /**< mutex that is used to access queue */
    pthread_cond_t cv;          /**< conditional variable used to signal that
                                 * job was finished  */
    pthread_mutex_t cv_mutex;   /**< mutex for queue conditional variable */
} queue_t;

/**
 * \brief Initialize new queue of jobs
 *
 * Initializes new queue of jobs including allocating memory for it.
 * \return new queue
 */
queue_t *queue_init();

/**
 * \brief Destroy queue of jobs
 *
 * Destroy existing queue of jobs including freeing memory it allocated.
 * \param q queue to destroy
 */
void queue_destroy(queue_t *q);

/**
 * \brief Run new job
 *
 * Finds new job in queue and starts it in separate thread.
 * \param q queue
 * \return job that was started
 */
job_t *queue_run_job(queue_t *q);

/**
 * \brief Add new job to queue
 *
 * Add new job of compressing block using specified window size.
 * \param q queue
 * \param block block to compress
 * \param window_size window size to use for compression
 */
void queue_add_job(queue_t *q, block_t *block,size_t window_size);

/**
 * \brief Fetch first finished job
 *
 * Find first finished job in queue, remove it from queue and return it.
 * \param q queue
 * \return first finished job
 */
job_t *queue_fetch_finished(queue_t *q);

/**
 * \brief Run queue
 *
 * Run queue by executing all of it's jobs. Jobs are executed in parallel using
 * separate threads.
 * \param q queue
 * \param threads max number of parallel threads to start
 */
void queue_run(queue_t *queue,int threads);

/**
 * \brief Destroy job
 *
 * Destroy single job including freeing it's memory.
 * \param job job to destroy
 */
void queue_destroy_job(job_t *job);

/** @} */
#endif
