#ifndef LZ77_H
#define LZ77_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "log.h"
#include "cfc.h"
#include "bit_string.h"

/** \addtogroup lz77
 * @{
 */

/** Maximal window size that this LZ77 implementation can use */
#define LZ77_MAX_NW 16777216

/**            
 * \brief Encode block bit string using LZ77
 * 
 * This function encodes bit string using LZ77 with window size nw and returns
 * new encoded bit string.
 * \param src source bit string to encode
 * \param nw window size to use when coding
 * \param window window from previous block or NULL if first block is encoded
 * \return new bit string holding encoded data
 */
bit_string_t *lz77_encode(bit_string_t *src, size_t nw, bit_string_t *window);

/**            
 * \brief Decode block bit string using LZ77
 * 
 * This function decodes bit string using LZ77 with window size nw and returns
 * new decoded bit string.
 * \param src source bit string to decode
 * \param size max size of decoded block
 * \param enc_size size of encoded block (output)
 * \param window window from previous block or NULL if first block is encoded
 * \param window_size window size - for first block window size is output, for subsequent
 * blocks it is input
 * \return new bit string holding encoded data
 */
bit_string_t *lz77_decode(bit_string_t *src, size_t size, size_t *enc_size, bit_string_t *window, size_t *window_size);

/** @} */
#endif
