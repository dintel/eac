#include "cfc.h"

size_t cfc_count_bits(uint32_t i)
{
    size_t result = 0;
    while(i) {
        ++result;
        i >>= 1;
    }
    return result;
}

void e(uint32_t k,bit_string_t *bs)
{
    size_t len = cfc_count_bits(k);
    //DEBUG_PRINT("e k %d len %zu cfc_count_bits(k) %d\n",k,len,cfc_count_bits(k));
    while(--len) {
        bit_string_append_bit(bs,0);
    }
    bit_string_append_bit(bs,1);
    for(int i = cfc_count_bits(k) - 1; i >= 0; --i) {
        bit_string_append_bit(bs, k >> i & 1);
    }
}

uint32_t e_inv(bit_string_t *bs, size_t *offset)
{
    uint32_t k = 0;
    uint32_t tmp;
    size_t len = bit_string_count_zeroes(bs,*offset) + 1;
    if(len + *offset >= bs->offset)
        return 0;
    PRINT_DEBUG("e_inv offset %zu len %zu\n",*offset,len);
    *offset += len;
    for(int i = 0; i < len; ++i) {
        tmp = bit_string_get_bit(bs, *offset);
        k = tmp | (k << 1);
        (*offset)++;
    }
    return k;
}

bit_string_t *cfc_encode(uint32_t l)
{
    size_t l_len = cfc_count_bits(l);
    bit_string_t *result = bit_string_init(cfc_count_bits(l_len) * 2 + l_len);
    PRINT_DEBUG("cfc_encode l %d l_len %zu\n",l,l_len);
    e(l_len,result);
    for(int i = l_len - 1; i >= 0; --i) {
        bit_string_append_bit(result, l >> i & 1);
    }
    return result;
}

size_t cfc_decode(bit_string_t *bs, size_t *offset)
{
    uint32_t l = 0, tmp;
    size_t l_len = e_inv(bs,offset);
    for(int i = 0; i < l_len; ++i) {
        tmp = bit_string_get_bit(bs,*offset);
        l = tmp | (l << 1);
        (*offset)++;
    }
    PRINT_DEBUG("cfc_decode offset %zu l %d l_len %zu\n",*offset - l_len,l,l_len);
    return l;
}
