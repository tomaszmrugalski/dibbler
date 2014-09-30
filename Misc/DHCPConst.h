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
#define LEASEQUERY_MSG       14
#define LEASEQUERY_REPLY_MSG 15

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

#define OPTION_RELAY_ID         53

// draft-ietf-softwire-ds-lite-tunnel-option-10, approved by IESG
#define OPTION_AFTR_NAME        64

// RFC6939
#define OPTION_CLIENT_LINKLAYER_ADDR 79

// draft-ietf-mif-dhcpv6-route-option-04
#define OPTION_NEXT_HOP         242
#define OPTION_RTPREFIX         243

// Experimental implementation for address prefix length information
// See: http://klub.com.pl/dhcpv6/doc/draft-mrugalski-addropts-XX-2007-04-17.txt
#define OPTION_ADDRPARAMS           251

// draft-mrugalski-remote-dhcpv6-00
#define OPTION_NEIGHBORS 254

// -- Query types (RFC5007) --
typedef enum {
    QUERY_BY_ADDRESS = 1,
    QUERY_BY_CLIENTID = 2
} ELeaseQueryType;

// --- Option lengths --
// (value of the len field, so actual option length is +4 bytes)
#define OPTION_ELAPSED_TIME_LEN     2
#define OPTION_INFORMATION_REFRESH_TIME_LEN         4

// --- Status Codes ---
/// @todo: convert this to enum
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

// INFINITY + 1 is 0. That's cool!
#define DHCPV6_INFINITY 0xffffffffu

/// used for 2 purposes:
/// is address tentative?
/// is address valid on link?
enum EAddrStatus {
    ADDRSTATUS_UNKNOWN = -1,
    ADDRSTATUS_NO      = 0,
    ADDRSTATUS_YES     = 1
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
    UNKNOWN_FQDN_APPEND = 3,      // accept, but append defined domain suffix
    UNKNOWN_FQDN_PROCEDURAL = 4   // generate name procedurally, append defined domain suffix
};

// defines Identity assotiation type
enum TIAType {
    IATYPE_IA, // IA_NA - non-temporary addresses
    IATYPE_TA, // IA_TA - temporary addresses
    IATYPE_PD  // IA_PD - prefix delegation
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

// Supported authorization protocols
enum AuthProtocols {
    AUTH_PROTO_NONE = 0,    // disabled
    AUTH_PROTO_DELAYED = 2, // RFC 3315
    AUTH_PROTO_RECONFIGURE_KEY = 3, // RFC 3315, section 21.5.1
    AUTH_PROTO_DIBBLER = 4 // Mechanism proposed by Kowalczuk
};

enum AuthReplay {
    AUTH_REPLAY_NONE = 0,
    AUTH_REPLAY_MONOTONIC = 1
};

// AUTH_ALGORITHM values for protocol type None (0)
// 0

// AUTH_ALGORITHM values for delayed auth (2)

// AUTH_ALGORITHM values for reconfigure key (3)
// This is protocol specific value and is useful only when AuthProtocols = 3
enum AuthAlgorithm_ReconfigureKey {
    AUTH_ALGORITHM_NONE = 0,
    AUTH_ALGORITHM_RECONFIGURE_KEY = 1
};

// AUTH_ALGORITHM values for protocol type Dibbler (4)
/// @todo: rename to AuthAlgorithm_DibblerDigestTypes
// This is protocol specific value and is useful only when AuthProtocols = 4
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

#ifdef __cplusplus
#include <vector>
typedef std::vector<DigestTypes> DigestTypesLst;
#endif

unsigned getDigestSize(enum DigestTypes type);
char *getDigestName(enum DigestTypes type);

// key is generated this way:
// key = HMAC-SHA1 (AAA-key, {Key Generation Nonce || client identifier})
// so it's size is always size of HMAC-SHA1 result which is 160bits = 20bytes
#define AUTHKEYLEN 20

// Values used in reconfigure-key algorithm (see RFC3315, section 21.5.1)
const static unsigned int RECONFIGURE_KEY_AUTHINFO_SIZE = 17;
const static unsigned int RECONFIGURE_KEY_SIZE = 16;    // HMAC-MD5 key
const static unsigned int RECONFIGURE_DIGEST_SIZE = 16; // HMAC-MD5 digest

// Values used in delayed-auth algorithm (see RFC3315, section 21.4)
const static unsigned int DELAYED_AUTH_KEY_SIZE = 16;   // HMAC-MD5 key
const static unsigned int DELAYED_AUTH_DIGEST_SIZE = 16; // HMAC-MD5 digest
const static unsigned int DELAYED_AUTH_KEY_ID_SIZE = 4; // uint32

#endif
