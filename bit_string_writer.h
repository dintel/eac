#ifndef BIT_STRING_WRITER_H
#define BIT_STRING_WRITER_H

#include "bit_string.h"

typedef struct {
    FILE *file;
    uint8_t buffer;
    size_t size;
} bit_string_writer_t;

bit_string_writer_t *bit_string_writer_init(FILE *file);
void bit_string_writer_destroy(bit_string_writer_t *writer);
size_t bit_string_writer_write(bit_string_writer_t *writer, bit_string_t *bs);
size_t bit_string_writer_flush(bit_string_writer_t *writer);

#endif
