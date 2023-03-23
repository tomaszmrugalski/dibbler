/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 * changes:
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <limits.h>

#ifndef DHCPDEFAULTS_H
#define DHCPDEFAULTS_H

// How long should we wait before we assume that OS detected duplicated
// addresses (in secs)
#define DADTIMEOUT ((unsigned long)3)

// addresses reported as DECLINED are not used for 2 hours
#define DECLINED_TIMEOUT ((unsigned long)7200)

// 1 (quiet) - 8 (debug)
#define DEFAULT_LOGLEVEL 7

// DHCPv6 server default values
#define SERVER_DEFAULT_DOMAIN ""
#define SERVER_DEFAULT_TIMEZONE ""
#define SERVER_DEFAULT_CLNTMAXLEASE 10
#define SERVER_DEFAULT_CLASSMAXLEASE 1048576
#define SERVER_DEFAULT_IFACEMAXLEASE UINT_MAX
#define SERVER_DEFAULT_PREFERENCE 0
#define SERVER_DEFAULT_RAPIDCOMMIT false
#define SERVER_DEFAULT_LEASEQUERY false
#define SERVER_DEFAULT_DNSUPDATE_MODE DNSUPDATE_MODE_NONE
#define SERVER_DEFAULT_DNSUPDATE_REVDNS_ZONE_LEN 64

#define SERVER_DEFAULT_MIN_T1 5
#define SERVER_DEFAULT_MAX_T1 3600 /* 1 hour */
#define SERVER_DEFAULT_MIN_T2 10
#define SERVER_DEFAULT_MAX_T2 5400      /* 1,5 hour */
#define SERVER_DEFAULT_MIN_PREF 7200    /* 2 hours */
#define SERVER_DEFAULT_MAX_PREF 86400   /* 1 day */
#define SERVER_DEFAULT_MIN_VALID 10800  /* 3 hours */
#define SERVER_DEFAULT_MAX_VALID 172800 /* 2 days */
#define SERVER_DEFAULT_CLASS_SHARE 100
#define SERVER_DEFAULT_CLASS_MAX_LEASE UINT_MAX
#define SERVER_DEFAULT_TA_PREF_LIFETIME 3600
#define SERVER_DEFAULT_TA_VALID_LIFETIME 7200
#define SERVER_DEFAULT_CACHE_SIZE 1048576 /* cache size, specified in bytes */

#define SERVER_MAX_IA_RANDOM_TRIES 100
#define SERVER_MAX_TA_RANDOM_TRIES 100
#define SERVER_MAX_PD_RANDOM_TRIES 100

// see DHCPConst.h for available enums
#define SERVER_DEFAULT_UNKNOWN_FQDN UNKNOWN_FQDN_REJECT

#define CLIENT_DEFAULT_T1 UINT_MAX
#define CLIENT_DEFAULT_T2 UINT_MAX

#define CLIENT_DEFAULT_UNICAST false
#define CLIENT_DEFAULT_RAPID_COMMIT false

// It is now /128. See discussion in bug #222
#define CLIENT_DEFAULT_PREFIX_LENGTH 128

#define CLIENT_DEFAULT_FQDN_FLAG_S true

#endif /* DHCPDEFAULTS_H */
