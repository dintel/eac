#ifndef BLOCK_H
#define BLOCK_H

#include <pthread.h>
#include <stdint.h>
#include "bit_string.h"

#define NUMBER_OF_WINDOW_SIZES 15

typedef struct block_t {
    bit_string_t *block;
    struct block_t *prev_block;
    struct block_t *next_block;
    bit_string_t *result;
    int num_block;
    size_t best_window_size;
    size_t checked_window_sizes;
    pthread_mutex_t mutex;
} block_t;

block_t *block_init(bit_string_t *block,block_t *prev, int num_block);
void block_destroy(block_t *block);

void block_update(block_t *block,size_t window_size, bit_string_t *result);
int block_is_complete(block_t *block);

#endif
