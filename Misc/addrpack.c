/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * some of those functions are taken form GNU libc6 library
 *
 * $Id: addrpack.c,v 1.12 2008-10-12 11:28:21 thomson Exp $
 */


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif

#include "stdint.h"

void print_packed(char addr[]);

#define NS_INADDRSZ     4       /* IPv4 T_A */
#define NS_IN6ADDRSZ    16      /* IPv6 T_AAAA */
#define NS_INT16SZ      2       /* #/bytes of data in a u_int16_t */
#define u_char unsigned char
#define u_int  unsigned int

static int inet_pton4(const char * src, char * dst)
{
	int saw_digit, octets, ch;
	u_char tmp[NS_INADDRSZ], *tp;

	saw_digit = 0;
	octets = 0;
	*(tp = tmp) = 0;
	while ((ch = *src++) != '\0') {

		if (ch >= '0' && ch <= '9') {
			u_int new = *tp * 10 + (ch - '0');

			if (new > 255)
				return (0);
			*tp = new;
			if (! saw_digit) {
				if (++octets > 4)
					return (0);
				saw_digit = 1;
			}
		} else if (ch == '.' && saw_digit) {
			if (octets == 4)
				return (0);
			*++tp = 0;
			saw_digit = 0;
		} else
			return (0);
	}
	if (octets < 4)
		return (0);
	memcpy(dst, tmp, NS_INADDRSZ);
	return (1);
}

int inet_pton6(const char *src, char * dst)
{
	static char xdigits[] = "0123456789abcdef";
	u_char tmp[NS_IN6ADDRSZ], *tp, *endp, *colonp;
	const char *curtok;
	int ch, saw_xdigit;
	u_int val;

	memset(tmp, '\0', NS_IN6ADDRSZ);
	tp = tmp;
	endp = tp + NS_IN6ADDRSZ;
	colonp = NULL;
	/* Leading :: requires some special handling. */
	if (*src == ':')
		if (*++src != ':')
			return (0);
	curtok = src;
	saw_xdigit = 0;
	val = 0;
	while ((ch = tolower (*src++)) != '\0') {
		char * pch;

		pch = strchr(xdigits, ch);
		if (pch != NULL) {
			val <<= 4;
			val |= (pch - xdigits);
			if (val > 0xffff)
				return (0);
			saw_xdigit = 1;
			continue;
		}
		if (ch == ':') {
			curtok = src;
			if (!saw_xdigit) {
				if (colonp)
					return (0);
				colonp = tp;
				continue;
			} else if (*src == '\0') {
				return (0);
			}
			if (tp + NS_INT16SZ > endp)
				return (0);
			*tp++ = (u_char) (val >> 8) & 0xff;
			*tp++ = (u_char) val & 0xff;
			saw_xdigit = 0;
			val = 0;
			continue;
		}
		if (ch == '.' && ((tp + NS_INADDRSZ) <= endp) &&
		    inet_pton4(curtok, (char*)tp) > 0) {
			tp += NS_INADDRSZ;
			saw_xdigit = 0;
			break;	/* '\0' was seen by inet_pton4(). */
		}
		return (0);
	}
	if (saw_xdigit) {
		if (tp + NS_INT16SZ > endp)
			return (0);
		*tp++ = (u_char) (val >> 8) & 0xff;
		*tp++ = (u_char) val & 0xff;
	}
	if (colonp != NULL) {
		/*
		 * Since some memmove()'s erroneously fail to handle
		 * overlapping regions, we'll do the shift by hand.
		 */
		const int n = tp - colonp;
		int i;

		if (tp == endp)
			return (0);
		for (i = 1; i <= n; i++) {
			endp[- i] = colonp[n - i];
			colonp[n - i] = 0;
		}
		tp = endp;
	}
	if (tp != endp)
		return (0);
	memcpy(dst, tmp, NS_IN6ADDRSZ);
	return (1);
}

char * inet_ntop4(const char * src, char * dst)
{
	char tmp[sizeof "255.255.255.255"];
	sprintf(tmp,"%u.%u.%u.%u", src[0], src[1], src[2], src[3]);
	return strcpy(dst, tmp);
}


char * inet_ntop6(const unsigned char * src, char * dst)
{
	char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
	struct { int base, len; } best, cur;
	u_int words[NS_IN6ADDRSZ / NS_INT16SZ];
	int i;
	best.len = cur.len = 0;

	/*
	 * Preprocess:
	 *	Copy the input (bytewise) array into a wordwise array.
	 *	Find the longest run of 0x00's in src[] for :: shorthanding.
	 */
	memset(words, '\0', sizeof words);
	for (i = 0; i < NS_IN6ADDRSZ; i += 2)
		words[i / 2] = (src[i] << 8) | src[i + 1];
	best.base = -1;
	cur.base = -1;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		if (words[i] == 0) {
			if (cur.base == -1)
				cur.base = i, cur.len = 1;
			else
				cur.len++;
		} else {
			if (cur.base != -1) {
				if (best.base == -1 || cur.len > best.len)
					best = cur;
				cur.base = -1;
			}
		}
	}
	if (cur.base != -1) {
		if (best.base == -1 || cur.len > best.len)
			best = cur;
	}
	if (best.base != -1 && best.len < 2)
		best.base = -1;

	/*
	 * Format the result.
	 */
	tp = tmp;
	for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
		/* Are we inside the best run of 0x00's? */
		if (best.base != -1 && i >= best.base &&
		    i < (best.base + best.len)) {
			if (i == best.base)
				*tp++ = ':';
			continue;
		}
		/* Are we following an initial run of 0x00s or any real hex? */
		if (i != 0)
			*tp++ = ':';
#if 0
		/* encapsulated IPv4 addresses are no concern in Dibbler */
		/* Is this address an encapsulated IPv4? */
		if (i == 6 && best.base == 0 &&
		    (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
			if (!inet_ntop4((char*)src+12, tp))
				return (NULL);
			tp += strlen(tp);
			break;
		}
#endif
		tp += sprintf(tp, "%x", words[i]);
	}
	/* Was it a trailing run of 0x00's? */
	if (best.base != -1 && (best.base + best.len) ==
	    (NS_IN6ADDRSZ / NS_INT16SZ))
		*tp++ = ':';
	*tp++ = '\0';
	return strcpy(dst, tmp);
}

void truncatePrefixFromConfig( char * src,  char * dst, char length){

	int i=0;
	dst[0]=0;
	
	for(i=0;i<length/8;i++) {
	    sprintf(dst + strlen(dst), "%02x", src[i]);
	    if (i && (i%2))
		sprintf(dst+strlen(dst), ":");
	}
	if (i%2)
	    sprintf(dst+strlen(dst), "::");
	else
	    sprintf(dst+strlen(dst), ":");
}

/** 
 * converts packed address to a reverse string used in DNS Updates
 * 
 * @param src - packed address
 * @param dst - output buffer (e.g. 1.2.3.0.0.0.0.0.0.0.0.0.0.e.f.f.3.ip6.arpa)
 */
void doRevDnsAddress( char * src,  char * dst){
	int i=0;
	dst[0]=0;
	
	for(i=15;i>=0;i--) {
	    sprintf(dst + strlen(dst), "%x.%x.", (src[i] & 0x0f), ( (src[i] & 0xf0 ) >> 4 ) );
	}
	sprintf(dst + strlen(dst), "ip6.arpa.");
}

/** 
 * function converts address to a DNS zone root with specified length
 * 
 * @param src - packed address, e.g. 3ffe::123
 * @param dst - dns zone root, e.g. 0.0.0.0.e.f.f.3.ip6.arpa
 * @param length - zone length, e.g. 96
 */
void doRevDnsZoneRoot( char * src,  char * dst, int length){
    int i=0;
    dst[0]=0;

    i = 15 - length/8; /* skip whole bytes */
    /** @todo: what to do with prefixes which do not divide by 4? */
    switch (length%8) {
    case 1:
	break;
    case 2:
	break;
    case 3:
	break;
    case 4:
	sprintf(dst + strlen(dst), "%x.", (src[i]&0xf0) >> 4);
	break;
    case 5:
	break;
    case 6:
	break;
    case 7:
    default:
	break;
    }
    if (length%8) i--;
    
    /* print the rest */
    for(; i>=0 ; i--) {
	sprintf(dst + strlen(dst), "%x.%x.", (src[i] & 0x0f), ( (src[i] & 0xf0 ) >> 4 ) );
    }
    sprintf(dst + strlen(dst), "ip6.arpa.");
}

void print_packed(char * addr)
{
    int i=0;
    for (;i<16;i++) {
	printf("%02x",*(unsigned char*)(addr+i));
	if ((i%2) && i<15) 
	    printf(":");
    }
   printf("\n");
} 

uint64_t htonll(uint64_t n) {
#if __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
    return (((uint64_t)htonl(n)) << 32) + htonl(n >> 32);
#endif
}

uint64_t ntohll(uint64_t n) {
#if __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
    return (((uint64_t)ntohl(n)) << 32) + ntohl(n >> 32);
#endif
}
