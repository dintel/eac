#include "bit_string.h"

void bit_string_print_bit(uint8_t byte,int offset)
{
    printf("%d",byte >> offset & 1);
}

void bit_string_print_byte(uint8_t byte,int bits)
{
    for(int i=0; i < bits; ++i) {
        bit_string_print_bit(byte,i);
    }
}

void bit_string_print(bit_string_t *bs,int bytes_in_row)
{
    int bits_in_row = bytes_in_row * 8;
    for(int i = 0; i < bs->offset; ++i) {
        if(i != 0 && i % bits_in_row == 0) {
            printf("\n");
        } else if (i != 0 && i % 8 == 0) {
            printf(" ");
        }
        printf("%d",bit_string_get_bit(bs,i));
    }
    printf("\n");
}

bit_string_t *bit_string_init(size_t size)
{
    bit_string_t *bs = malloc(sizeof(bit_string_t));
    bs->data = malloc(sizeof(uint8_t)*size);
    bs->size = size;
    bs->offset = 0;
    return bs;
}

void bit_string_destroy(bit_string_t *bs)
{
    if(bs == NULL)
        return;
    free(bs->data);
    free(bs);
}

int bit_string_cmp(bit_string_t *bs1,bit_string_t *bs2)
{
    int i = 0;
    while(bit_string_get_bit(bs1,i) == bit_string_get_bit(bs2,i) && i < bs1->offset && i < bs2->offset) ++i;
    return i;
}

int bit_string_sub_cmp(bit_string_t *bs1,bit_string_t *bs2,size_t offset1, size_t offset2)
{
    int i = 0;
    while(offset1+i < bs1->offset && offset2+i < bs2->offset && bit_string_get_bit(bs1,offset1+i) == bit_string_get_bit(bs2,offset2+i)) ++i;
    return i;
}

size_t bit_string_count_zeroes(bit_string_t *bs, size_t offset)
{
    size_t i = 0;
    while(bit_string_get_bit(bs,offset + i) == 0 && offset + i < bs->offset) ++i;
    return i;
}

void bit_string_append_bit(bit_string_t *bs,uint8_t bit)
{
    int i = bs->offset / 8;
    bs->data[i] = (bs->data[i] << 1) | (bit & 1);
    bs->offset++;
}

uint8_t bit_string_get_bit(bit_string_t *bs,size_t index)
{
    if(index/8 == bs->size)
        printf("%zu %zu %zu\n",bs->size,index,bs->offset);
    uint8_t byte = bs->data[index/8];
    if(index >= bs->offset - bs->offset % 8 && index <= bs->offset) {
        return (byte >> ((bs->offset % 8) - index % 8 - 1)) & 1; 
    }
    return (byte >> (7 - index % 8)) & 1;
}

bit_string_t *bit_string_substr(bit_string_t *src,int offset, size_t size)
{
    bit_string_t *result = bit_string_init(src->size);
    for(int i = 0; i < size; i++) {
        bit_string_append_bit(result,bit_string_get_bit(src,offset + i));
    }
    return result;
}

void bit_string_concat(bit_string_t *dst,bit_string_t *src)
{
    for(int i = 0; i < src->offset; ++i) {
        bit_string_append_bit(dst,bit_string_get_bit(src,i));
    }
}

void bit_string_concat_and_destroy(bit_string_t *dst,bit_string_t *src)
{
    bit_string_concat(dst,src);
    bit_string_destroy(src);
}

void bit_string_copy(bit_string_t *dst, bit_string_t *src, int offset, size_t len)
{
    for(int i = 0; i < len; ++i) {
        if(dst->offset >= dst->size * 8 || offset + i >= src->offset) {
            PRINT_DEBUG("ERROR in bit_string_copy (dst->offset %zu dst->size * 8 %zu)!\n",dst->offset,dst->size * 8);
            break;
        }
        bit_string_append_bit(dst,bit_string_get_bit(src,offset + i));
        PRINT_DEBUG("bit_string_copy copied bit %x\n",bit_string_get_bit(src,offset + i));
    }
}

uint8_t bit_string_read_byte(bit_string_t *bs, int offset, size_t limit)
{
    uint8_t result = 0;
    for(int i = 0; i < limit; ++i) {
        result |= bit_string_get_bit(bs,offset + i) << i;
    }
    return result;
}

void bit_string_full_copy(bit_string_t *src, bit_string_t *dst)
{
    for(int i = 0; i < src->size; i++) {
        dst->data[i] = src->data[i];
    }
    for(int i = src->size; i < dst->size; i++) {
        dst->data[i] = 0;
    }
    dst->offset = src->offset;
}
