#ifndef BLOCK_H
#define BLOCK_H

#include <pthread.h>
#include <stdint.h>
#include "bit_string.h"

#define NUMBER_OF_WINDOW_SIZES 15

/** \addtogroup block
 * @{
 */

/**
 * \brief block
 *
 * This type represents single block of data in file. It holds both source data
 * and best compressed variant of data. When working against this type with
 * block_* functions it is thread safe. Additionally all blocks form a double
 * linked list.
 */
typedef struct block_t {
    bit_string_t *block;         /**< bit string of data */
    struct block_t *prev_block;  /**< pointer to previous block */
    struct block_t *next_block;  /**< pointer to next block */
    bit_string_t *result;        /**< best compressed result of block */
    int num_block;               /**< sequential number of block */
    size_t best_window_size;     /**< window size used to get best result */
    size_t checked_window_sizes; /**< number of window sizes that were checked */
    pthread_mutex_t mutex;       /**< mutex that is locked to change block */
    size_t longest_match;        /**< longest match length */
} block_t;

/**
 * \brief Initialize new block
 *
 * Allocate memory for new block and initialize it.
 * \param block bit string holding bits of data
 * \param prev previous block of data
 * \param num_block sequential number of block in file
 * \return new block
 */
block_t *block_init(bit_string_t *block,block_t *prev, int num_block);

/**
 * \brief Destroy block
 *
 * Destroy block by freeing memory it allocated.
 * \param block block to destroy
 */
void block_destroy(block_t *block);

/**
 * \brief Update block result
 *
 * Checks whether new result for block is better than existing. If yes result
 * and best window size of block are updated with new values. Since result is
 * allocated during initialization, this function actually copies data into
 * result without allocating memory.
 * \param block block to update
 * \param window_size window size of new result
 * \param result new result
 * \param longest_match longest match length
 */
void block_update(block_t *block,size_t window_size, bit_string_t *result, size_t longest_match);

/**
 * \brief Check whether block is completed
 *
 * Block is considered to be complete when all possible window sizes for it's
 * compression were checked and best one was chosen.
 * \param block block to check
 * \return 1 if complete, 0 otherwise
 */
int block_is_complete(block_t *block);

/** @} */
#endif
