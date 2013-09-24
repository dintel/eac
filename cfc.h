#ifndef CFC_H
#define CFC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "log.h"
#include "bit_string.h"

/** \addtogroup cfc
 * @{
 */

/**
 * \brief Encode number using CFC
 * 
 * This function can encode any 32bit unsigned number using CFC. It returns
 * newly allocated \ref bit_string that must be freed by user when not needed
 * anymore.
 * \param l number to encode
 * \return bit_string that contains CFC of l
 */
bit_string_t *cfc_encode(uint32_t l);

/**
 * \brief Decode CFC encoded number
 * 
 * This function decodes CFC encoded number.
 * \param bs bit_string where CFC encoded number is stored
 * \param offset offset where CFC encoded number begins. Updated to offset
 * immediately after CFC encoded number.
 * \return number that was encoded using CFC
 */
size_t cfc_decode(bit_string_t *bs, size_t *offset);

/** @} */
#endif
