#include "block.h"

block_t *block_init(bit_string_t *b,block_t *prev, int num_block)
{
    block_t *block = malloc(sizeof(block_t));
    block->block = b;
    block->prev_block = prev;
    block->next_block = NULL;
    block->result = bit_string_init(b->size * 4);;
    block->num_block = num_block;
    block->best_window_size = 0;
    block->checked_window_sizes = 0;
    if(prev != NULL)
        prev->next_block = block;
    pthread_mutex_init(&block->mutex, NULL);
    return block;
}

void block_destroy(block_t *block)
{
    bit_string_destroy(block->block);
    bit_string_destroy(block->result);
    pthread_mutex_destroy(&block->mutex);
    free(block);
}

void block_update(block_t *block,size_t window_size, bit_string_t *result)
{
    pthread_mutex_lock(&block->mutex);
    block->checked_window_sizes++;
    if(block->result->offset == 0 || block->result->offset > result->offset) {
        bit_string_full_copy(result,block->result);
        block->best_window_size = window_size;
    }
    pthread_mutex_unlock(&block->mutex);
}

int block_is_incomplete(block_t *block)
{
    pthread_mutex_lock(&block->mutex);
    int incomplete = NUMBER_OF_WINDOW_SIZES - block->checked_window_sizes;
    pthread_mutex_unlock(&block->mutex);
    return incomplete;
}
