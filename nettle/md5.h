/* md5.h
 *
 * The MD5 hash function, described in RFC 1321.
 */

/* nettle, low-level cryptographics library
 *
 * Copyright (C) 2001 Niels M�ller
 *
 * The nettle library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * The nettle library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the nettle library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef NETTLE_MD5_H_INCLUDED
#define NETTLE_MD5_H_INCLUDED

#include "nettle-types.h"

/* Name mangling */
#define md5_init nettle_md5_init
#define md5_update nettle_md5_update
#define md5_digest nettle_md5_digest

#define MD5_DIGEST_SIZE 16
#define MD5_DATA_SIZE 64

/* Digest is kept internally as 4 32-bit words. */
#define _MD5_DIGEST_LENGTH 4

struct md5_ctx {
  uint32_t digest[_MD5_DIGEST_LENGTH];
  uint32_t count_l, count_h;    /* Block count */
  uint8_t block[MD5_DATA_SIZE]; /* Block buffer */
  unsigned index;               /* Into buffer */
};

void md5_init(struct md5_ctx *ctx);

void md5_update(struct md5_ctx *ctx, unsigned length, const uint8_t *data);

void md5_digest(struct md5_ctx *ctx, unsigned length, uint8_t *digest);

#endif /* NETTLE_MD5_H_INCLUDED */
