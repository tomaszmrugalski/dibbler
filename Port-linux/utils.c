/*
 * utils.c
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 *
 * Authors:	Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 *
 *
 * Changes:
 *
 * Rani Assaf <rani@magic.metawire.com> 980929:	resolve addresses
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <resolv.h>
//#include <linux/pkt_sched.h>

#include "utils.h"

int default_scope(inet_prefix *lcl)
{
	if (lcl->family == AF_INET) {
		if (lcl->bytelen >= 1 && *(__u8*)&lcl->data == 127)
			return RT_SCOPE_HOST;
	}
	return 0;
}

const char *rt_addr_n2a(int af, int len, void *addr, char *buf, int buflen)
{
    if (af==AF_INET || af==AF_INET6)
	return inet_ntop(af, addr, buf, buflen);
    else
	return "Nieobs�ugiwana rodzina adres�w";
}



int get_integer(int *val, char *arg, int base)
{
	long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtol(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > INT_MAX || res < INT_MIN)
		return -1;
	*val = res;
	return 0;
}

int get_unsigned(unsigned *val, char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtoul(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > UINT_MAX)
		return -1;
	*val = res;
	return 0;
}

int get_u32(__u32 *val, char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtoul(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > 0xFFFFFFFFUL)
		return -1;
	*val = res;
	return 0;
}

int get_u16(__u16 *val, char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtoul(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > 0xFFFF)
		return -1;
	*val = res;
	return 0;
}

int get_u8(__u8 *val, char *arg, int base)
{
	unsigned long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtoul(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > 0xFF)
		return -1;
	*val = res;
	return 0;
}

int get_s16(__s16 *val, char *arg, int base)
{
	long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtol(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > 0x7FFF || res < -0x8000)
		return -1;
	*val = res;
	return 0;
}

int get_s8(__s8 *val, char *arg, int base)
{
	long res;
	char *ptr;

	if (!arg || !*arg)
		return -1;
	res = strtol(arg, &ptr, base);
	if (!ptr || ptr == arg || *ptr || res > 0x7F || res < -0x80)
		return -1;
	*val = res;
	return 0;
}

int get_addr_1(inet_prefix *addr, char *name, int family)
{
	char *cp;
	unsigned char *ap = (unsigned char*)addr->data;
	int i;

	memset(addr, 0, sizeof(*addr));

	if (strcmp(name, "default") == 0 ||
	    strcmp(name, "all") == 0 ||
	    strcmp(name, "any") == 0) {
		if (family == AF_DECnet)
			return -1;
		addr->family = family;
		addr->bytelen = (family == AF_INET6 ? 16 : 4);
		addr->bitlen = -1;
		return 0;
	}

	if (strchr(name, ':')) {
		addr->family = AF_INET6;
		if (family != AF_UNSPEC && family != AF_INET6)
			return -1;
		if (inet_pton(AF_INET6, name, addr->data) <= 0)
			return -1;
		addr->bytelen = 16;
		addr->bitlen = -1;
		return 0;
	}

	addr->family = AF_INET;
	if (family != AF_UNSPEC && family != AF_INET)
		return -1;
	addr->bytelen = 4;
	addr->bitlen = -1;
	for (cp=name, i=0; *cp; cp++) {
		if (*cp <= '9' && *cp >= '0') {
			ap[i] = 10*ap[i] + (*cp-'0');
			continue;
		}
		if (*cp == '.' && ++i <= 3)
			continue;
		return -1;
	}
	return 0;
}

int get_prefix_1(inet_prefix *dst, char *arg, int family)
{
	int err;
	unsigned plen;
	char *slash;

	memset(dst, 0, sizeof(*dst));

	if (strcmp(arg, "default") == 0 || strcmp(arg, "any") == 0) {
		if (family == AF_DECnet)
			return -1;
		dst->family = family;
		dst->bytelen = 0;
		dst->bitlen = 0;
		return 0;
	}

	slash = strchr(arg, '/');
	
	if (slash)
		*slash = 0;

	err = get_addr_1(dst, arg, family);

	if (err == 0) {
		switch(dst->family) {
			case AF_INET6:
				dst->bitlen = 128;
				break;
			case AF_DECnet:
				dst->bitlen = 16;
				break;
			default:
			case AF_INET:
				dst->bitlen = 32;
		}
		if (slash) {
			if (get_integer(&plen, slash+1, 0) || plen > dst->bitlen) {
				err = -1;
				goto done;
			}
			dst->bitlen = plen;
		}
	}
done:
	if (slash)
		*slash = '/';
	return err;
}

int get_addr(inet_prefix *dst, char *arg, int family)
{
	if (family == AF_PACKET) {
		fprintf(stderr, "Error: \"%s\" may be inet address, but it is not allowed in this context.\n", arg);
		exit(1);
	}
	if (get_addr_1(dst, arg, family)) {
		fprintf(stderr, "Error: an inet address is expected rather than \"%s\".\n", arg);
		exit(1);
	}
	return 0;
}

int get_prefix(inet_prefix *dst, char *arg, int family)
{
	if (family == AF_PACKET) {
		fprintf(stderr, "Error: \"%s\" may be inet prefix, but it is not allowed in this context.\n", arg);
		exit(1);
	}
	if (get_prefix_1(dst, arg, family)) {
		fprintf(stderr, "Error: an inet prefix is expected rather than \"%s\".\n", arg);
		exit(1);
	}
	return 0;
}

__u32 get_addr32(char *name)
{
	inet_prefix addr;
	if (get_addr_1(&addr, name, AF_INET)) {
		fprintf(stderr, "Error: an IP address is expected rather than \"%s\"\n", name);
		exit(1);
	}
	return addr.data[0];
}

void incomplete_command()
{
	fprintf(stderr, "Command line is not complete. Try option \"help\"\n");
	exit(-1);
}

void invarg(char *msg, char *arg)
{
	fprintf(stderr, "Error: argument \"%s\" is wrong: %s\n", arg, msg);
	exit(-1);
}

void duparg(char *key, char *arg)
{
	fprintf(stderr, "Error: duplicate \"%s\": \"%s\" is the second value.\n", key, arg);
	exit(-1);
}

void duparg2(char *key, char *arg)
{
	fprintf(stderr, "Error: either \"%s\" is duplicate, or \"%s\" is a garbage.\n", key, arg);
	exit(-1);
}

int matches(char *cmd, char *pattern)
{
	int len = strlen(cmd);
	if (len > strlen(pattern))
		return -1;
	return memcmp(pattern, cmd, len);
}

int inet_addr_match(inet_prefix *a, inet_prefix *b, int bits)
{
	__u32 *a1 = a->data;
	__u32 *a2 = b->data;
	int words = bits >> 0x05;

	bits &= 0x1f;

	if (words)
		if (memcmp(a1, a2, words << 2))
			return -1;

	if (bits) {
		__u32 w1, w2;
		__u32 mask;

		w1 = a1[words];
		w2 = a2[words];

		mask = htonl((0xffffffff) << (0x20 - bits));

		if ((w1 ^ w2) & mask)
			return 1;
	}

	return 0;
}

int __iproute2_hz_internal;

int __get_hz(void)
{
	int hz = 0;
	FILE *fp = fopen("/proc/net/psched", "r");

	if (fp) {
		unsigned nom, denom;
		if (fscanf(fp, "%*08x%*08x%08x%08x", &nom, &denom) == 2)
			if (nom == 1000000)
				hz = denom;
		fclose(fp);
	}
	if (hz)
		return hz;
	return HZ;
}


const char *format_host(int af, int len, void *addr, char *buf, int buflen)
{
#ifdef RESOLVE_HOSTNAMES
	if (resolve_hosts) {
		struct hostent *h_ent;
		if (len <= 0) {
			switch (af) {
			case AF_INET:
				len = 4;
				break;
			case AF_INET6:
				len = 16;
				break;
			case AF_IPX:
				len = 10;
				break;
#ifdef AF_DECnet
			/* I see no reasons why gethostbyname
			   may not work for DECnet */
			case AF_DECnet:
				len = 2;
				break;
#endif
			default: ;
			}
		}
		if (len > 0 &&
		    (h_ent = gethostbyaddr(addr, len, af)) != NULL) {
			snprintf(buf, buflen-1, "%s", h_ent->h_name);
			return buf;
		}
	}
#endif
	return rt_addr_n2a(af, len, addr, buf, buflen);
}
