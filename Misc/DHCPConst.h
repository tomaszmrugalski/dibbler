/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: DHCPConst.h,v 1.15 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2004/11/30 00:43:24  thomson
 * SIP option value mismatch fixed (bug #68)
 *
 * Revision 1.13  2004/11/29 17:42:33  thomson
 * Update option values to RFC3898
 *
 * Revision 1.12  2004/10/27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.11  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.10  2004/10/03 21:52:12  thomson
 * FALSE changed to false
 *
 * Revision 1.9  2004/10/03 21:21:32  thomson
 * CLIENT_DEFAULT_UNICAST is set to false.
 *
 * Revision 1.8  2004/09/28 21:49:32  thomson
 * no message
 *
 * Revision 1.7  2004/09/07 22:02:34  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.6  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.5  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.4  2004/06/04 19:03:47  thomson
 * Resolved warnings with signed/unisigned
 *
 * Revision 1.3  2004/03/29 22:06:49  thomson
 * 0.1.1 version
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
#define OPTION_IAADDR           5
#define OPTION_ORO              6
#define OPTION_PREFERENCE       7
#define OPTION_ELAPSED_TIME     8
#define OPTION_UNICAST          12
#define OPTION_STATUS_CODE      13
#define OPTION_RAPID_COMMIT     14

// RFC: options not supported yet
#define OPTION_IA_TA            4
#define OPTION_RELAY_MSG        9
#define OPTION_AUTH_MSG         11
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

// RFC3633: Prefix options (not supported yet)
#define OPTION_IA_PD            25
#define OPTION_IAPREFIX         26

// RFC3898: NIS options
#define OPTION_NIS_SERVERS	27
#define OPTION_NISP_SERVERS	28
#define OPTION_NIS_DOMAIN_NAME  29
#define OPTION_NISP_DOMAIN_NAME 30

// draft-ietf-dhc-dhcpv6-opt-timeconfig-02.txt
#define OPTION_NTP_SERVERS      40
#define OPTION_TIME_ZONE        41

// draft-ietf-dhc-lifetime-02.txt
#define OPTION_LIFETIME         42

// draft-ietf-dhc-dhcpv6-fqdn-00.txt
#define OPTION_FQDN             43

// --- Status Codes --- 
#define STATUSCODE_SUCCESS       0
#define STATUSCODE_UNSPECFAIL    1
#define STATUSCODE_NOADDRSAVAIL  2
#define STATUSCODE_NOBINDING     3
#define STATUSCODE_NOTONLINK     4
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

#define CLIENT_DEFAULT_T1 ULONG_MAX
#define CLIENT_DEFAULT_T2 ULONG_MAX

#define CLIENT_DEFAULT_UNICAST false
#define CLIENT_DEFAULT_RAPID_COMMIT false

enum ETentative {YES, NO, DONTKNOWYET};
enum EState {NOTCONFIGURED, INPROCESS, CONFIGURED, FAILED, DISABLED, UNKNOWN, TENTATIVECHECK, TENTATIVE};

// FQDN option flags
#define FQDN_N 0x4
#define FQDN_O 0x2
#define FQDN_S 0x1
    
bool allowOptInOpt(int msgType, int optOut, int optIn);
bool allowOptInMsg(int msgType, int optType);

#ifdef WIN32
#define uint16_t unsigned short int
#define uint32_t unsigned int
#endif

#endif

