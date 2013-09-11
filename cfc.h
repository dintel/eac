#ifndef CFC_H
#define CFC_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "log.h"
#include "bit_string.h"

bit_string_t *cfc_encode(uint32_t l);
size_t cfc_decode(bit_string_t *bs, size_t *offset);

#endif
