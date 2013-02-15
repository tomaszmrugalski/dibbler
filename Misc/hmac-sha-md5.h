/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */	

#ifdef __cplusplus
extern "C" {
#endif

void *hmac_sha (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf, int type);
void *hmac_md5 (const char *buffer, size_t len, char *key, size_t key_len, char *resbuf);

#ifdef __cplusplus
}
#endif

