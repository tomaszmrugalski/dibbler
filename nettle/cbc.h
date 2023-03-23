/* cbc.h
 *
 * Cipher block chaining mode.
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

#ifndef NETTLE_CBC_H_INCLUDED
#define NETTLE_CBC_H_INCLUDED

#include "nettle-types.h"

/* Name mangling */
#define cbc_encrypt nettle_cbc_encrypt
#define cbc_decrypt nettle_cbc_decrypt

/* Uses a void * for cipher contexts.

   For block ciphers it would make sense with a const void * for the
   context, but we use the same typedef for stream ciphers where the
   internal state changes during the encryption. */
typedef void (*nettle_crypt_func)(void *ctx, unsigned length, uint8_t *dst,
                                  const uint8_t *src);

void cbc_encrypt(void *ctx, nettle_crypt_func f, unsigned block_size,
                 uint8_t *iv, unsigned length, uint8_t *dst,
                 const uint8_t *src);

void cbc_decrypt(void *ctx, nettle_crypt_func f, unsigned block_size,
                 uint8_t *iv, unsigned length, uint8_t *dst,
                 const uint8_t *src);

#define CBC_CTX(type, size)                                                    \
  {                                                                            \
    type ctx;                                                                  \
    uint8_t iv[size];                                                          \
  }

#define CBC_SET_IV(ctx, data) memcpy((ctx)->iv, (data), sizeof((ctx)->iv))

#if 0
#define CBC_ENCRYPT(self, f, length, dst, src)                                 \
  do {                                                                         \
    if (0)                                                                     \
      (f)(&(self)->ctx, 0, NULL, NULL);                                        \
    cbc_encrypt((void *)&(self)->ctx,                                          \
                (void (*)(void *, unsigned, uint8_t *, const uint8_t *))(f),   \
                sizeof((self)->iv), (self)->iv, (length), (dst), (src));       \
  } while (0)
#endif

#define CBC_ENCRYPT(self, f, length, dst, src)                                 \
  (0 ? ((f)(&(self)->ctx, 0, NULL, NULL))                                      \
     : cbc_encrypt(                                                            \
           (void *)&(self)->ctx,                                               \
           (void (*)(void *, unsigned, uint8_t *, const uint8_t *))(f),        \
           sizeof((self)->iv), (self)->iv, (length), (dst), (src)))

#define CBC_DECRYPT(self, f, length, dst, src)                                 \
  (0 ? ((f)(&(self)->ctx, 0, NULL, NULL))                                      \
     : cbc_decrypt(                                                            \
           (void *)&(self)->ctx,                                               \
           (void (*)(void *, unsigned, uint8_t *, const uint8_t *))(f),        \
           sizeof((self)->iv), (self)->iv, (length), (dst), (src)))

#if 0
/* Type safer variants */
#define CBC_ENCRYPT2(ctx, f, b, iv, l, dst, src)                               \
  (0 ? ((f)((ctx), 0, NULL, NULL))                                             \
     : cbc_encrypt(                                                            \
           (void *)(ctx),                                                      \
           (void (*)(void *, unsigned, uint8_t *, const uint8_t *))(f), (b),   \
           (iv), (l), (dst), (src)))

#endif

#endif /* NETTLE_CBC_H_INCLUDED */
