/* memxor.h
 *
 */

#ifndef NETTLE_MEMXOR_H_INCLUDED
#define NETTLE_MEMXOR_H_INCLUDED

#include "nettle-types.h"
#include <stdlib.h>

uint8_t *memxor(uint8_t *dst, const uint8_t *src, size_t n);

#endif /* NETTLE_MEMXOR_H_INCLUDED */
