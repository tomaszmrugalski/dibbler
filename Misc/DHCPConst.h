/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * Released under GNU GPL v2 licence
 *
 */

#include "DHCPDefaults.h"

#ifndef DHCPCONST_H
#define DHCPCONST_H

#define ALL_DHCP_RELAY_AGENTS_AND_SERVERS  "ff02::1:2"
#define ALL_DHCP_SERVERS                   "ff05::1:3"

#define DHCPCLIENT_PORT 546
#define DHCPSERVER_PORT 547

// messages
#define SOLICIT_MSG    1
#define ADVERTISE_MSG  2
#define REQUEST_MSG    3
#define CONFIRM_MSG    4
#define RENEW_MSG      5
#define REBIND_MSG     6
#define REPLY_MSG      7
#define RELEASE_MSG    8
#define DECLINE_MSG    9
#define RECONFIGURE_MSG 10
#define INFORMATION_REQUEST_MSG 11
#define RELAY_FORW_MSG 12
#define RELAY_REPL_MSG 13
#define LEASEQUERY_MSG 14
#define LEASEQUERY_REPLY_MSG 15
#define LEASEQUERY_DONE_MSG 16
#define LEASEQUERY_DATA_MSG 17

// implementation specific
#define CONTROL_MSG    255

// timers, timeouts

#define SOL_MAX_DELAY 1
#define SOL_TIMEOUT   1
#define SOL_MAX_RT    120
#define REQ_TIMEOUT   1
#define REQ_MAX_RT    30
#define REQ_MAX_RC    10
#define CNF_MAX_DELAY 1
#define CNF_TIMEOUT   1
#define CNF_MAX_RT    4
#define CNF_MAX_RD    10
#define REN_TIMEOUT   10
#define REN_MAX_RT    600
#define REB_TIMEOUT   10
#define REB_MAX_RT    600
#define INF_MAX_DELAY 1
#define INF_TIMEOUT   1
#define INF_MAX_RT    120
#define REL_TIMEOUT   1
#define REL_MAX_RC    5
#define DEC_TIMEOUT   1
#define DEC_MAX_RC    5
#define REC_TIMEOUT   2
#define REC_MAX_RC    8

#define HOP_COUNT_LIMIT 32

// how long does server caches its replies?
#define SERVER_REPLY_CACHE_TIMEOUT 60

// RFC3315: supported options
#define OPTION_CLIENTID         1
#define OPTION_SERVERID         2
#define OPTION_IA_NA            3
#define OPTION_IA_TA            4
#define OPTION_IAADDR           5
#define OPTION_ORO              6
#define OPTION_PREFERENCE       7
#define OPTION_ELAPSED_TIME     8
#define OPTION_UNICAST          12
#define OPTION_STATUS_CODE      13
#define OPTION_RAPID_COMMIT     14

// RFC3315: options not supported yet
#define OPTION_RELAY_MSG        9
#define OPTION_AUTH             11
#define OPTION_USER_CLASS       15
#define OPTION_VENDOR_CLASS     16
#define OPTION_VENDOR_OPTS      17
#define OPTION_INTERFACE_ID     18
#define OPTION_RECONF_MSG       19
#define OPTION_RECONF_ACCEPT    20

// additional options

// RFC3319: SIP servers and domains
#define OPTION_SIP_SERVER_D      21
#define OPTION_SIP_SERVER_A      22

// RFC3646: DNS servers and domains
#define OPTION_DNS_SERVERS	23
#define OPTION_DOMAIN_LIST      24

// RFC3633: Prefix options
#define OPTION_IA_PD            25
#define OPTION_IAPREFIX         26

// RFC3898: NIS options
#define OPTION_NIS_SERVERS	27
#define OPTION_NISP_SERVERS	28
#define OPTION_NIS_DOMAIN_NAME  29
#define OPTION_NISP_DOMAIN_NAME 30

// RFC4075: Simple Network Time Protocol (SNTP)
#define OPTION_SNTP_SERVERS     31

// RFC4242: Information Refresh Time Option
#define OPTION_INFORMATION_REFRESH_TIME     32

// RFC4280: Broadcast and Multicast Control Servers
#define OPTION_BCMCS_SERVER_D   33
#define OPTION_BCMCS_SERVER_A   34

// RFC4776: Option for Civic Addresses Configuration Information
#define OPTION_GEOCONF_CIVIC    36

// RFC4649: Relay Agent Remote-ID Option
#define OPTION_REMOTE_ID        37

// RFC4580: Relay Agent Subscriber-ID Option
#define OPTION_SUBSCRIBER_ID    38

// RFC4704: Client Fully Qualified Domain Name (FQDN) Option
#define OPTION_FQDN             39

// RFC-ietf-dhc-paa-option-05.txt
#define OPTION_PANA_AGENT       40

// RFC4833: Timezone options for DHCP
#define OPTION_NEW_POSIX_TIMEZONE 41
#define OPTION_NEW_TZDB_TIMEZONE  42

// RFC-ietf-dhc-dhcpv6-ero-01.txt
#define OPTION_ERO              43

// RFC5007: Leasequery
#define OPTION_LQ_QUERY         44
#define OPTION_CLIENT_DATA      45
#define OPTION_CLT_TIME         46
#define OPTION_LQ_RELAY_DATA    47
#define OPTION_LQ_CLIENT_LINK   48

// draft-ietf-softwire-ds-lite-tunnel-option-10, approved by IESG
#define OPTION_AFTR_NAME        64

// RFC5460: Bulk Leasequer
#define OPTION_RELAY_ID		53

// The following option numbers are not yet standardized and
// won't interoperate with other implementations
// option formats taken from:
// draft-ram-dhc-dhcpv6-aakey-01.txt
#define OPTION_AAAAUTH              240
#define OPTION_KEYGEN               241

// Experimental implementation for address prefix length information
// See: http://klub.com.pl/dhcpv6/doc/draft-mrugalski-addropts-XX-2007-04-17.txt
#define OPTION_ADDRPARAMS           251

// draft-mrugalski-remote-dhcpv6-00
#define OPTION_NEIGHBORS 254

// -- Query types (RFC5007) --
typedef enum {
    QUERY_BY_ADDRESS = 1,
    QUERY_BY_CLIENT_ID = 2,
    QUERY_BY_RELAY_ID = 3,
    QUERY_BY_LINK_ADDRESS = 4,
    QUERY_BY_REMOTE_ID = 5
} ELeaseQueryType;

// --- Option lengths --
// (value of the len field, so actual option length is +4 bytes)
#define OPTION_ELAPSED_TIME_LEN     2
#define OPTION_INFORMATION_REFRESH_TIME_LEN         4

// --- Status Codes ---
#define STATUSCODE_SUCCESS          0
#define STATUSCODE_UNSPECFAIL       1
#define STATUSCODE_NOADDRSAVAIL     2
#define STATUSCODE_NOBINDING        3
#define STATUSCODE_NOTONLINK        4
#define STATUSCODE_USEMULTICAST     5
#define STATUSCODE_NOPREFIXAVAIL    6
// Leasequery status codes
#define STATUSCODE_UNKNOWNQUERYTYPE 7
#define STATUSCODE_MALFORMEDQUERY   8
#define STATUSCODE_NOTCONFIGURED    9
#define STATUSCODE_NOTALLOWED       10
// Bulk Leasequery status codes
#define STATUSCODE_QUERYTERMINATED 11

// INFINITY + 1 is 0. That's cool!
#define DHCPV6_INFINITY (unsigned) 0xffffffff

enum ETentative {
    TENTATIVE_UNKNOWN = -1,
    TENTATIVE_NO      = 0,
    TENTATIVE_YES     = 1
};

enum EState {
    STATE_NOTCONFIGURED,
    STATE_INPROCESS,
    STATE_CONFIGURED,
    STATE_FAILED,
    STATE_DISABLED,
    STATE_CONFIRMME,
    STATE_TENTATIVECHECK,
    STATE_TENTATIVE};

// specifies server behavior, when receiving unknown FQDN
enum EUnknownFQDNMode {
    UNKNOWN_FQDN_REJECT = 0,      // reject unknown FQDNs (do not assign a name from pool)
    UNKKOWN_FQDN_ACCEPT_POOL = 1, // assign other name available in pool
    UNKNOWN_FQDN_ACCEPT = 2,      // accept unknown FQDNs
    UKNNOWN_FQDN_APPEND = 3,      // accept, but append defined domain suffix
    UKNNOWN_FQDN_PROCEDURAL = 4   // generate name procedurally, append defined domain suffix
};

// FQDN option flags
#define FQDN_N 0x4
#define FQDN_O 0x2
#define FQDN_S 0x1

// --- Bitfield in ADDRPARAMS option ---
#define ADDRPARAMS_MASK_PREFIX    0x01
#define ADDRPARAMS_MASK_ANYCAST   0x02
#define ADDRPARAMS_MASK_MULTICAST 0x04


int allowOptInOpt(int msgType, int optOut, int optIn);
int allowOptInMsg(int msgType, int optType);

#ifdef WIN32
//#define uint8_t  unsigned char
//#define uint16_t unsigned short int
//#define uint32_t unsigned int

#ifndef uint8_t
#define uint8_t  unsigned char
#endif

#ifndef uint16_t
#define uint16_t unsigned short int
#endif

#ifndef uint32_t
#define uint32_t unsigned int
#endif

#ifndef uint64_t
#define uint64_t unsigned long long int
#endif

#else
#include <stdint.h>
#endif

enum DigestTypes {
	DIGEST_NONE = 0,
	DIGEST_PLAIN = 1,
	DIGEST_HMAC_MD5 = 2,
	DIGEST_HMAC_SHA1 = 3,
	DIGEST_HMAC_SHA224 = 4,
	DIGEST_HMAC_SHA256 = 5,
	DIGEST_HMAC_SHA384 = 6,
	DIGEST_HMAC_SHA512 = 7,
	//this must be last, increase it if necessary
	DIGEST_INVALID = 8
};

unsigned getDigestSize(enum DigestTypes type);
char *getDigestName(enum DigestTypes type);

// key is generated this way:
// key = HMAC-SHA1 (AAA-key, {Key Generation Nonce || client identifier})
// so it's size is always size of HMAC-SHA1 result which is 160bits = 20bytes
#define AUTHKEYLEN 20

#endif
