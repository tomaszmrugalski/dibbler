/* Declaration of functions and data types used for MD5 sum computing
   library functions.
   Copyright (C) 1995-1997,1999,2000,2001,2004,2005,2006
      Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* This file is taken from coreutils-6.2 (lib/md5.h) and adapted for dibbler
 * by Michal Kowalczuk <michal@kowalczuk.eu> */

#ifndef _MD5_H
#define _MD5_H 1

#include <stdio.h>
#include <stdint.h>

#define MD5_BLOCKSIZE 64
#define MD5_DIGESTSIZE 16

#ifndef __GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
#  define __GNUC_PREREQ(maj, min)					\
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
# else
#  define __GNUC_PREREQ(maj, min) 0
# endif
#endif

#ifndef __THROW
# if defined __cplusplus && __GNUC_PREREQ (2,8)
#  define __THROW	throw ()
# else
#  define __THROW
# endif
#endif

#ifndef _LIBC
# define __md5_buffer md5_buffer
# define __md5_finish_ctx md5_finish_ctx
# define __md5_init_ctx md5_init_ctx
# define __md5_process_block md5_process_block
# define __md5_process_bytes md5_process_bytes
# define __md5_read_ctx md5_read_ctx
#endif

/* Structure to save state of computation between the single steps.  */
struct md5_ctx
{
  uint32_t A;
  uint32_t B;
  uint32_t C;
  uint32_t D;

  uint32_t total[2];
  uint32_t buflen;
  uint32_t buffer[32];
};

/* Initialize structure containing state of computation.
   (RFC 1321, 3.3: Step 3)  */
extern void __md5_init_ctx (struct md5_ctx *ctx) __THROW;

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is necessary that LEN is a multiple of 64!!! */
extern void __md5_process_block (const void *buffer, size_t len,
				 struct md5_ctx *ctx) __THROW;

/* Starting with the result of former calls of this function (or the
   initialization function update the context for the next LEN bytes
   starting at BUFFER.
   It is NOT required that LEN is a multiple of 64.  */
extern void __md5_process_bytes (const void *buffer, size_t len,
				 struct md5_ctx *ctx) __THROW;

/* Process the remaining bytes in the buffer and put result from CTX
   in first 16 bytes following RESBUF.  The result is always in little
   endian byte order, so that a byte-wise output yields to the wanted
   ASCII representation of the message digest.

   IMPORTANT: On some systems, RESBUF must be aligned to a 32-bit
   boundary. */
extern void *__md5_finish_ctx (struct md5_ctx *ctx, void *resbuf) __THROW;


/* Put result from CTX in first 16 bytes following RESBUF.  The result is
   always in little endian byte order, so that a byte-wise output yields
   to the wanted ASCII representation of the message digest.

   IMPORTANT: On some systems, RESBUF must be aligned to a 32-bit
   boundary. */
extern void *__md5_read_ctx (const struct md5_ctx *ctx, void *resbuf) __THROW;


#endif /* md5.h */
