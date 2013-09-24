#include <stdlib.h>
#include "bit_string_writer.h"

bit_string_writer_t *bit_string_writer_init(FILE *file)
{
    bit_string_writer_t *writer = malloc(sizeof(bit_string_writer_t));
    writer->file = file;
    writer->buffer = 0;
    writer->size = 0;
    return writer;
}

void bit_string_writer_destroy(bit_string_writer_t *writer)
{
    free(writer);
}

size_t bit_string_writer_write(bit_string_writer_t *writer, bit_string_t *bs)
{
    bit_string_t *tmp = bit_string_init(bs->size + 16);
    for(int i = writer->size - 1; i >= 0; --i) {
        bit_string_append_bit(tmp, writer->buffer >> i & 1);
    }
    for(int i = 0; i < bs->offset; ++i) {
        bit_string_append_bit(tmp, bit_string_get_bit(bs,i));
    }
    size_t result = fwrite(tmp->data,sizeof(uint8_t),tmp->offset / 8, writer->file);
    PRINT_DEBUG("bit_string_writer_write wrote %zu bits\n",tmp->offset - tmp->offset % 8);
    writer->buffer = tmp->data[tmp->offset / 8];
    writer->size = tmp->offset % 8;
    bit_string_destroy(tmp);
    if(writer->size == 0) {
        writer->buffer = 0;
    }
    return result * 8;
}

size_t bit_string_writer_flush(bit_string_writer_t *writer)
{
    size_t size = writer->size;
    uint8_t buffer = writer->buffer;
    buffer <<= (8 - size);
    if(size != 0)
        fwrite(&(buffer),1,1,writer->file);
    PRINT_DEBUG("bit_string_writer_flush flushed %zu bits (%x %x)\n",size,writer->buffer,buffer);
    writer->buffer = 0;
    writer->size = 0;
    return size;
}

void bit_string_writer_write_byte(bit_string_writer_t *writer, uint8_t byte, uint8_t limit)
{
    bit_string_t *bs = bit_string_init(1);
    for(int i = 0; i < limit; ++i) {
        PRINT_DEBUG("byte_string_witer_write_byte write bit %d\n",byte >> i & 1);
        bit_string_append_bit(bs, byte >> i & 1);
    }
    bit_string_writer_write(writer,bs);
    bit_string_destroy(bs);
}
