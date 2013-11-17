#ifndef BIT_STRING_H
#define BIT_STRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "log.h"

/** \addtogroup bit_string
 * @{
 */

/**
 * \brief bit string
 *
 * This type is used to work with strings consisting of single bits.
 */
typedef struct {
    uint8_t *data;              /**< array of bytes holding actual data */
    size_t size;                /**< number of bytes allocated for this string */
    size_t offset;              /**< number of bits stored in this string */
} bit_string_t;

/**
 * \brief Initialize new bit string
 *
 * Allocate new bit string and space for storing bit string data. Size of bit
 * string data is specified in <b>bytes</b>.
 * \param size number of bytes to allocate for data
 * \return new bit string
 */
bit_string_t *bit_string_init(size_t size);

/**
 * \brief Destroy bit string
 *
 * Free memory allocated to bit string and it's data.
 * \param bs bit string to destroy
 */
void bit_string_destroy(bit_string_t *bs);

/**
 * \brief Compare 2 bit strings
 *
 * Compares 2 bit strings and returns number of bits that are identical in both
 * bit strings starting on first bit.
 * \param bs1 first bit string
 * \param bs2 second bit string
 * \return length of common substring starting at position 0
 */
int bit_string_cmp(bit_string_t *bs1, bit_string_t *bs2);

/**
 * \brief Compare 2 bit substrings
 *
 * Compares 2 bit strings starting at offset1 and offset2 respectively.
 * \param bs1 first bit string
 * \param bs2 second bit string
 * \param offset1 position from which bits in first string should be compared
 * \param offset2 position from which bits in second string should be compared
 * \return number of consecutive bits identical in both string
 */
int bit_string_sub_cmp(bit_string_t *bs1,bit_string_t *bs2,size_t offset1, size_t offset2);

/**
 * \brief Count number of 0 bits
 *
 * Returns number of consecutive 0 bits starting at offset.
 * \param bs bit string
 * \param offset offset to count from
 * \return number of consecutive 0 bits
 */
size_t bit_string_count_zeroes(bit_string_t *bs, size_t offset);

/**
 * \brief Append bit to bit string
 *
 * \param bs bit string
 * \param bit bit to append
 */
void bit_string_append_bit(bit_string_t *bs, uint8_t bit);

/**
 * \brief Get value of bit at some index
 *
 * \param bs bit string
 * \param index index of bit
 * \return value of bit (0 or 1)
 */
uint8_t bit_string_get_bit(bit_string_t *bs, size_t index);

/**
 * \brief Get substring of bit string
 *
 * Create new bit string containing substring of given bit string. New bit
 * string will have same number of bytes allocated for storage as source bit
 * string.
 *
 * <b>NOTE:</b> It's user's responsibility to destroy returned bit string
 * \param src source bit string
 * \param offset offset from which substring should start
 * \param size number of bits that should be copied to substing
 * \return substring - 'size' bits starting at 'offset'
 */
bit_string_t *bit_string_substr(bit_string_t *src, int offset, size_t size);

/**
 * \brief Append one bit string to another
 *
 * This function copies all bits from one bit string to end of another with
 * same order. Bit string that bits are copied to <b>must</b> have enough space
 * to store all bits.
 * \param dst Destination bit string
 * \param src Source bit string
 */
void bit_string_concat(bit_string_t *dst,bit_string_t *src);

/**
 * \brief Append one bit string to another and destroy source bit string
 *
 * This function copies all bits from source bit string to end of destination
 * bit string and than destroys source bit string. Destination bit string
 * <b>must</b> have enough free space to store all bits.
 * \param dst Destination bit string
 * \param src Source bit string
 */
void bit_string_concat_and_destroy(bit_string_t *dst,bit_string_t *src);

/**
 * \brief Copy substring of bit string to another bit string
 *
 * This function appends 'len' bits starting at 'offset' from source bit string
 * to destination bit string.
 * \param dst Destination bit string
 * \param src Source bit string
 * \param offset offset from which bits from source bit string should be copied
 * \param len number of bits that should be copied from source bit string
 */
void bit_string_copy(bit_string_t *dst, bit_string_t *src, int offset, size_t len);

/**
 * \brief Read up to 8 bits into byte
 *
 * Helper function that allows to read up to 8 bits at once using single byte.
 * \param bs bit string
 * \param offset offset from which bits should be read
 * \param limit number of bits to read
 * \return byte containing specified bits
 */
uint8_t bit_string_read_byte(bit_string_t *bs, int offset, size_t limit);

/**
 * \brief Print bit string
 *
 * Prints bit string. Bits are grouped in groups of 8 bits. Each group is
 * separated by space. Number of bytes printed in each row is controllable
 * through parameter.
 * \param bs bit string
 * \param bytes_in_row number of bytes in row to print
 */
void bit_string_print(bit_string_t *bs,int bytes_in_row);

/**
 * \brief Copy bit string into another bit string
 *
 * Copies bit string into another existing bit string. Target bit string must be
 * large enough to hold source bit string.
 * \param src source bit string
 * \param dst destination bit string
 */
void bit_string_full_copy(bit_string_t *src, bit_string_t *dst);

/** @} */
#endif
