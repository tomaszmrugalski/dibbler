#ifdef __cplusplus
extern "C" {
#endif

void *hmac_sha (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf, int type);
void *hmac_md5 (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf);

#ifdef __cplusplus
}
#endif

