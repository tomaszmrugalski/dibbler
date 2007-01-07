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
#define OPTION_IA               3
#define OPTION_IA_TA            4
#define OPTION_IAADDR           5
#define OPTION_ORO              6
#define OPTION_PREFERENCE       7
#define OPTION_ELAPSED_TIME     8
#define OPTION_UNICAST          12
#define OPTION_STATUS_CODE      13
#define OPTION_RAPID_COMMIT     14

// RFC: options not supported yet
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
#define OPTION_SIP_DOMAINS      21
#define OPTION_SIP_SERVERS      22

// RFC3646: DNS servers and domains
#define OPTION_DNS_RESOLVERS	23
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
#define OPTION_NTP_SERVERS      31

// draft-ietf-dhc-lifetime-02.txt
#define OPTION_LIFETIME         32

// draft-ietf-dhc-dhcpv6-fqdn-00.txt
#define OPTION_FQDN             34

// (obsolete) draft-ietf-dhc-dhcpv6-opt-timeconfig-02.txt
#define OPTION_TIME_ZONE        41

// --- Option lengths --
// (value of the len field, so actual option length is +4 bytes)
#define OPTION_ELAPSED_TIME_LEN     2
#define OPTION_LIFETIME_LEN         4

// --- Status Codes --- 
#define STATUSCODE_SUCCESS       0
#define STATUSCODE_UNSPECFAIL    1
#define STATUSCODE_NOADDRSAVAIL  2
#define STATUSCODE_NOBINDING     3
#define STATUSCODE_NOTONLINK     4
#define STATUSCODE_NOPREFIXAVAIL 6 
#define STATUSCODE_USEMULTICAST  5

// INFINITY + 1 is 0. Hih, cool
#define DHCPV6_INFINITY (unsigned) 0xffffffff

// how long should we wait before we assume that OS detected duplicated addresses (in secs)
#define DADTIMEOUT (unsigned) 1

// addresses reported ad DECLINED are not used for 2 hours
#define DECLINED_TIMEOUT (unsigned) 7200

// DHCPv6 server default values
#define SERVER_DEFAULT_DOMAIN ""
#define SERVER_DEFAULT_TIMEZONE ""
#define SERVER_DEFAULT_CLNTMAXLEASE ULONG_MAX
#define SERVER_DEFAULT_IFACEMAXLEASE ULONG_MAX
#define SERVER_DEFAULT_PREFERENCE 0
#define SERVER_DEFAULT_RAPIDCOMMIT false

#define SERVER_DEFAULT_MIN_T1    5
#define SERVER_DEFAULT_MAX_T1    ULONG_MAX
#define SERVER_DEFAULT_MIN_T2    10
#define SERVER_DEFAULT_MAX_T2    ULONG_MAX
#define SERVER_DEFAULT_MIN_PREF  1800
#define SERVER_DEFAULT_MAX_PREF  86400     /* 1 day */
#define SERVER_DEFAULT_MIN_VALID 3600
#define SERVER_DEFAULT_MAX_VALID 172800    /* 2 days */
#define SERVER_DEFAULT_CLASS_SHARE 100
#define SERVER_DEFAULT_CLASS_MAX_LEASE ULONG_MAX
#define SERVER_DEFAULT_TA_PREF_LIFETIME 3600
#define SERVER_DEFAULT_TA_VALID_LIFETIME 7200
#define SERVER_DEFAULT_CACHE_SIZE 1048576   /* cache size, specified in bytes */
#define CLIENT_DEFAULT_T1 ULONG_MAX
#define CLIENT_DEFAULT_T2 ULONG_MAX

#define CLIENT_DEFAULT_UNICAST false
#define CLIENT_DEFAULT_RAPID_COMMIT false

#define CLIENT_DEFAULT_PREFIX_LENGTH 64
#define CLIENT_DEFAULT_DIGEST DIGEST_NONE

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
    STATE_TENTATIVECHECK, 
    STATE_TENTATIVE};

// FQDN option flags
#define FQDN_N 0x4
#define FQDN_O 0x2
#define FQDN_S 0x1
    
int allowOptInOpt(int msgType, int optOut, int optIn);
int allowOptInMsg(int msgType, int optType);

#ifdef WIN32
#define uint16_t unsigned short int
#define uint32_t unsigned int
#endif

enum DigestTypes {
        DIGEST_NONE = 0,
        DIGEST_PLAIN = 1,
        DIGEST_HMAC_MD5 = 2,
        DIGEST_HMAC_SHA1 = 3,
        DIGEST_HMAC_SHA224 = 4,
        DIGEST_HMAC_SHA256 = 5,
        DIGEST_HMAC_SHA384 = 6,
        DIGEST_HMAC_SHA512 = 7
};

unsigned getDigestSize(enum DigestTypes type);
        
// for debugging purposes:
#define printhex(buf, len) { unsigned j; for (j = 0; j < len; j++) printf("%02x ", (unsigned char) *(buf+j)); printf("\n"); }

#endif

