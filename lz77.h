#ifndef LZ77_H
#define LZ77_H

#include <stddef.h>
#include <stdint.h>
#include <math.h>
#include "log.h"
#include "cfc.h"
#include "bit_string.h"

#define LZ77_MAX_NW 16777216

bit_string_t *lz77_encode(bit_string_t *src, size_t nw);
bit_string_t *lz77_decode(bit_string_t *src, size_t size, size_t *enc_size);

#endif
