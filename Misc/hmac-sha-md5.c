/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: hmac-sha-md5.c,v 1.1 2006-11-30 03:28:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 *
 */

#include <string.h>
#include <stdlib.h>
#include "hmac.h"
#include "md5.h"
#include "sha1.h"
#include "sha256.h"
#include "sha512.h"

/* "Feeding The Void With Emptiness..." ;) */
#define SHA_CASE(x,y)                                                                       \
          case x:                                                                           \
                  ctx = malloc(sizeof(struct sha##y##_ctx));                                \
                  init_ctx      = (void(*)(void *))                &sha##x##_init_ctx;      \
                  process_bytes = (void(*)(void *, size_t, void *))&sha##y##_process_bytes; \
                  finish_ctx    = (void* (*)(void *, void *))      &sha##x##_finish_ctx;    \
                  blocksize = SHA##x##_BLOCKSIZE;                                           \
                  digestsize = SHA##x##_DIGESTSIZE;                                         \
                  break;

/* Take buffer and key (and their lengths), generate HMAC-SHA (or HMAC-MD5)     */
/* and write the result to RESBUF                                               */
/* type is one of the following: 1, 224, 256, 384, 512 (for SHA) or 5 (for MD5) */
static void *
hmac_sha_md5 (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf, int type) {
  /* SHA512_BLOCKSIZE and SHA512_DIGESTSIZE are the biggest, so we can use it with other algorithms */
  char Ki[SHA512_BLOCKSIZE];
  char Ko[SHA512_BLOCKSIZE];
  char tmpbuf[SHA512_DIGESTSIZE];
  int i;
  int blocksize;
  int digestsize;
  void *ctx;
  void (*init_ctx)(void *);
  void (*process_bytes)(void *, size_t, void *);
  void *(*finish_ctx)(void *, void *);
  void *result;

  switch (type) {
          case 5: /* Oh, it's MD5! */
                  ctx = malloc(sizeof(struct md5_ctx));
                  init_ctx      = (void(*)(void *))                 &md5_init_ctx;
                  process_bytes = (void(*)(void *, size_t, void *)) &md5_process_bytes;
                  finish_ctx    = (void* (*)(void *, void *))       &md5_finish_ctx;
                  blocksize = MD5_BLOCKSIZE;
                  digestsize = MD5_DIGESTSIZE;
                  break;
               /*   .--------< SHA variant   
                    |                        
                    v                   */
          SHA_CASE(  1,   1)
          SHA_CASE(224, 256)
          SHA_CASE(256, 256)
          SHA_CASE(384, 512)
          SHA_CASE(512, 512)
          default:
                  return NULL;
  }

  /* if given key is longer that algorithm's block, we must change it to
     hash of the original key (of size of algorithm's digest) */
  if (key_len > blocksize) {
          init_ctx (&ctx);
          process_bytes (key, key_len, &ctx);
          finish_ctx (&ctx, Ki);
          key_len = digestsize;
          memcpy(Ko, Ki, key_len);
  } else {
          memcpy(Ki, key, key_len);
          memcpy(Ko, key, key_len);
  }

  /* prepare input and output key */
  for (i = 0; i < key_len; i++) {
          Ki[i] ^= 0x36;
          Ko[i] ^= 0x5c;
  }
  for (; i < blocksize; i++) {
          Ki[i] = 0x36;
          Ko[i] = 0x5c;
  }

  init_ctx (ctx);
  process_bytes (Ki, blocksize, ctx);
  process_bytes ((void *)buffer, len, ctx);
  finish_ctx (ctx, tmpbuf);

  init_ctx (ctx);
  process_bytes (Ko, blocksize, ctx);
  process_bytes (tmpbuf, digestsize, ctx);

  result = finish_ctx (ctx, resbuf);
  free(ctx);

  return result;
}

/* HMAC-SHA function wrapper */
void *hmac_sha (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf, int type) {
        return hmac_sha_md5(buffer, len, key, key_len, resbuf, type);
}

/* HMAC-MD5 function wrapper */
void *hmac_md5 (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf) {
        return hmac_sha_md5(buffer, len, key, key_len, resbuf, 5);
}
