#ifndef DHCPCONST_H
#define DHCPCONST_H

// adresy

#define ALL_DHCP_RELAY_AGENTS_AND_SERVERS  "ff02::1:2"
#define ALL_DHCP_SERVERS                   "ff05::1:3"

#define DHCPCLIENT_PORT 546
#define DHCPSERVER_PORT 547

// wiadomosci
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
#define RELAY_FORW     12
#define RELAY_REPL     13
#define CONTROL_MSG    255

// op�nienia,retransmisje

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

// opcje
#define OPTION_CLIENTID 1
#define OPTION_SERVERID 2
#define OPTION_IA       3
#define OPTION_IAADDR   5
#define OPTION_ORO      6
#define OPTION_PREFERENCE 7
#define OPTION_ELAPSED_TIME 8
#define OPTION_UNICAST 12
#define OPTION_STATUS_CODE 13
#define OPTION_RAPID_COMMIT 14
// not supported yet
#define OPTION_IA_TA    4
#define OPTION_RELAY_MSG 9
#define OPTION_AUTH_MSG 11
#define OPTION_USER_CLASS 15
#define OPTION_VENDOR_CLASS 16
#define OPTION_VENDOR_OPTS 17
#define OPTION_INTERFACE_ID 18
#define OPTION_RECONF_MSG 19
#define OPTION_RECONF_ACCEPT 20

// draft-ietf-dhc-dhcpv6-opt-dnsconfig-03.txt
#define OPTION_DNS_RESOLVERS	23
#define OPTION_DOMAIN_LIST      24

// draft-ietf-dhc-dhcpv6-opt-timeconfig-02.txt
#define OPTION_NTP_SERVERS 40
#define OPTION_TIME_ZONE   41

// currently not used
#define OPTION_NIS_SERVERS	35
#define OPTION_NISP_SERVERS	36
#define OPTION_NIS_DOMAIN_NAME  37
#define OPTION_NISP_DOMAIN_NAME 38

// --- Status Codes --- 
#define STATUSCODE_SUCCESS       0
#define STATUSCODE_UNSPECFAIL    1
#define STATUSCODE_NOADDRSAVAIL  2
#define STATUSCODE_NOBINDING     3
#define STATUSCODE_NOTONLINK     4
#define STATUSCODE_USEMULTICAST  5

// INFINITY + 1 is 0. Hih, cool
#define INFINITY 0xffffffff

// how long should we wait before we assume that OS detected duplicated addresses 
// default (3 secs)
#define DADTIMEOUT 3

// addresses reported ad DECLINED are not used for 2 hours
#define DECLINED_TIMEOUT 7200

enum ESendOpt {Send,Default,Supersede};
enum EReqOpt {Request, Require};
enum ETentative {YES, NO, DONTKNOWYET};

enum EState { NOTCONFIGURED, INPROCESS, CONFIGURED, FAILED, UNKNOWN };
    
bool canBeOptInOpt(int msgType, int optOut, int optIn);
bool canBeOptInMsg(int msgType, int optType);

#define List(x) TContainer< SmartPtr< x > > 

#ifdef WIN32
#define uint16_t unsigned short int
#define uint32_t unsigned int
#endif

#endif

