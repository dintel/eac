#ifndef BIT_STRING_H
#define BIT_STRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "log.h"

typedef struct {
    uint8_t *data;
    size_t size;
    size_t offset;
} bit_string_t;

bit_string_t *bit_string_init(size_t size);
void bit_string_destroy(bit_string_t *bs);

int bit_string_cmp(bit_string_t *bs1, bit_string_t *bs2);
int bit_string_sub_cmp(bit_string_t *bs1,bit_string_t *bs2,size_t offset1, size_t offset2);
size_t bit_string_count_zeroes(bit_string_t *bs, size_t offset);
void bit_string_append_bit(bit_string_t *bs, uint8_t bit);
uint8_t bit_string_get_bit(bit_string_t *bs, size_t index);
bit_string_t *bit_string_substr(bit_string_t *src, int offset, size_t size);
void bit_string_concat(bit_string_t *dst,bit_string_t *src);
void bit_string_concat_and_destroy(bit_string_t *dst,bit_string_t *src);
void bit_string_copy(bit_string_t *dst, bit_string_t *src, int offset, size_t len);

void bit_string_print(bit_string_t *bs,int bytes_in_row);

#endif
