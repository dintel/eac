#ifndef BIT_STRING_WRITER_H
#define BIT_STRING_WRITER_H

#include "bit_string.h"

/** \addtogroup bit_string_writer
 * @{
 */

/**
 * \brief Bit string writer
 *
 * Structure holding information about bit string writer.
 */
typedef struct {
    FILE *file;                 /**< File into which bits are written */
    uint8_t buffer;             /**< One byte buffer for bits that are not yet written */
    size_t size;                /**< Number of bits stored in buffer */
} bit_string_writer_t;

/**
 * \brief Initialize new bit string writer
 *
 * Creates \ref bit_string_writer_t structure.
 * \param file File into which new writer will write.
 * \return new bit string writer
 */
bit_string_writer_t *bit_string_writer_init(FILE *file);

/**
 * \brief Destring bit string writer
 *
 * Free memory allocated for specified bit string writer.
 * \param writer bit string writer
 */
void bit_string_writer_destroy(bit_string_writer_t *writer);

/**
 * \brief Write bit string
 *
 * Write bit string using bit string writer.
 * \param writer bit string writer
 * \param bs bit string
 * \return number of <b>bits</b> actually written to disk
 */
size_t bit_string_writer_write(bit_string_writer_t *writer, bit_string_t *bs);

/**
 * \brief Flush bit string writer buffer onto disk
 *
 * Save bits that are remaining in buffer onto disk and reset buffer.
 * \param writer bit string writer
 * \return number of bits written to disk
 */
size_t bit_string_writer_flush(bit_string_writer_t *writer);

/**
 * \brief Write some bits of byte using bit string writer
 *
 * This is a helper function that writes limit bits from byte.
 * \param writer bit string writer
 * \param byte byte from which bits should be taken
 * \param limit number of bits to extract and write
 */
void bit_string_writer_write_byte(bit_string_writer_t *writer, uint8_t byte, uint8_t limit);

/** @} */
#endif
