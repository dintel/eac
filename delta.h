#ifndef DELTA_H
#define DELTA_H

#include <stdint.h>

/** Minimal window size allowed */
#define MIN_NW 8

/** Maximal window size allowed */
#define MAX_NW 262144

/** Maximum delta of log2 of window size (currently not used) */
#define MAX_NW_DELTA 15

/** Number of bits used by encoded window size change */
#define NW_BITS 5

/** \addtogroup delta_nw
 * @{
 */

/**
 * \brief Output encoded change in window size
 *
 * Currently encodes log2 window size using NW_BITS in byte and returns it. It
 * would be more efficient to calculate delta between window size changes. Delta
 * value is how many bits should be shifted right/left. \f$\delta = \log_2\left(\frac{\max\{nw_{n-1},nw_n\}}{\min\{nw_{n-1},nw_n\}}\right)\f$
 * \param prev previous block window size
 * \param next next block window size
 * \return encoded new window size
 */
uint8_t nw_change_encode(int prev,int next);

/**
 * \brief Decode window size of next block
 *
 * Calculate window size of next block. Currently decodes number stored in delta
 * and returns 2 in power of that number. More efficient way is to code delta of
 * window size (or even log2 of delta), but currently the implementation is the
 * simplest one.
 * \param prev window size of previous block
 * \param delta encoded window size
 * \return new window size
 */
int nw_change_decode(int prev, uint8_t delta);

/** @} */

#endif
