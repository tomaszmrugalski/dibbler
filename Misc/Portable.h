/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: Portable.h,v 1.101 2009-03-09 22:27:23 thomson Exp $
 */	

#ifndef PORTABLE_H
#define PORTABLE_H

/* #define DIBBLER_VERSION "0.7.3-CVS (" __DATE__ " " __TIME__ ")" */
#define DIBBLER_VERSION "0.7.3"

#define DIBBLER_COPYRIGHT1 "| Dibbler - a portable DHCPv6, version " DIBBLER_VERSION
#define DIBBLER_COPYRIGHT2 "| Authors : Tomasz Mrugalski<thomson(at)klub.com.pl>,Marek Senderski<msend(at)o2.pl>"
#define DIBBLER_COPYRIGHT3 "| Licence : GNU GPL v2 only. Developed at Gdansk University of Technology."
#define DIBBLER_COPYRIGHT4 "| Homepage: http://klub.com.pl/dhcpv6/"

#ifdef LINUX
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <resolv.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#endif 

#ifdef WIN32
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

#define snprintf _snprintf
#endif

#ifdef MACOS
#include <stdint.h>
#endif

#define DEFAULT_UMASK 027

/**********************************************************************/
/*** data structures **************************************************/
/**********************************************************************/

#ifndef MAX_IFNAME_LENGTH 
#define MAX_IFNAME_LENGTH 255
#endif

/* Structure used for interface name reporting */
struct iface {
    char name[MAX_IFNAME_LENGTH];  /* interface name */
    int  id;                       /* interface ID (often called ifindex) */
    int  hardwareType;             /* type of hardware (see RFC 826) */
    char mac[255];                 /* link layer address */
    int  maclen;                   /* length of link layer address */
    char *linkaddr;                /* assigned IPv6 link local addresses  */
    int  linkaddrcount;            /* number of assigned IPv6 link local addresses */
    char *globaladdr;              /* global IPv6 addresses */
    int  globaladdrcount;          /* number of global IPV6 addresses */
    unsigned int flags;            /* look IF_xxx in portable.h */
    struct iface* next;            /* structure describing next iface in system */
};

#define MAX_LINK_STATE_CHANGES_AT_ONCE 16

struct link_state_notify_t
{
    int ifindex[MAX_LINK_STATE_CHANGES_AT_ONCE]; /* indexes of interfaces that has changed. Only non-zero values will be used */
    int cnt;  /* number of iterface indexes filled */
};

/**********************************************************************/
/*** file setup/default paths *****************************************/
/**********************************************************************/

#define CLNTCFGMGR_FILE   "client-CfgMgr.xml"
#define CLNTIFACEMGR_FILE "client-IfaceMgr.xml"
#define CLNTDUID_FILE	  "client-duid"
#define CLNTADDRMGR_FILE  "client-AddrMgr.xml"
#define CLNTTRANSMGR_FILE "client-TransMgr.xml"

#define SRVCFGMGR_FILE    "server-CfgMgr.xml"
#define SRVIFACEMGR_FILE  "server-IfaceMgr.xml"
#define SRVDUID_FILE	  "server-duid"
#define SRVADDRMGR_FILE   "server-AddrMgr.xml"
#define SRVTRANSMGR_FILE  "server-TransMgr.xml"
#define SRVCACHE_FILE     "server-cache.xml"

#define RELCFGMGR_FILE    "relay-CfgMgr.xml"
#define RELIFACEMGR_FILE  "relay-IfaceMgr.xml"
#define RELTRANSMGR_FILE  "relay-TransMgr.xml"

#define REQIFACEMGR_FILE  "requestor-IfaceMgr.xml"

#define DNSUPDATE_DEFAULT_TTL "2h"
#define INACTIVE_MODE_INTERVAL 3 /* 3 seconds */

#define REQLOG_FILE        "dibbler-requestor.log"

#ifdef WIN32
#define WORKDIR            ".\\"
#define DEFAULT_SCRIPTSDIR ".\\scritps"
#define CLNTCONF_FILE 	   "client.conf"
#define SRVCONF_FILE       "server.conf"
#define RELCONF_FILE       "relay.conf"
#define CLNTLOG_FILE       "dibbler-client.log"
#define SRVLOG_FILE        "dibbler-server.log"
#define RELLOG_FILE        "dibbler-relay.log"
#define NULLFILE           "nul"
#endif

#ifdef LINUX
#define WORKDIR            "/var/lib/dibbler"
#define DEFAULT_SCRIPTSDIR "/var/lib/dibbler/scripts"
#define CLNTCONF_FILE      "/etc/dibbler/client.conf"
#define SRVCONF_FILE       "/etc/dibbler/server.conf"
#define RELCONF_FILE       "/etc/dibbler/relay.conf"
#define RESOLVCONF_FILE    "/etc/resolv.conf"
#define NTPCONF_FILE       "/etc/ntp.conf"
#define TIMEZONE_FILE      "/etc/localtime"
#define TIMEZONES_DIR      "/usr/share/zoneinfo"
#define RADVD_FILE         "/etc/dibbler/radvd.conf"
#define CLNTPID_FILE       "/var/lib/dibbler/client.pid"
#define SRVPID_FILE        "/var/lib/dibbler/server.pid"
#define RELPID_FILE        "/var/lib/dibbler/relay.pid"
#define CLNTLOG_FILE       "/var/log/dibbler/dibbler-client.log"
#define SRVLOG_FILE        "/var/log/dibbler/dibbler-server.log"
#define RELLOG_FILE        "/var/log/dibbler/dibbler-relay.log"
#define NULLFILE           "/dev/null"
#endif

#ifdef MACOS
#define WORKDIR            "/var/lib/dibbler"
#define DEFAULT_SCRIPTSDIR "/var/lib/dibbler/scripts"
#define CLNTCONF_FILE      "/etc/dibbler/client.conf"
#define SRVCONF_FILE       "/etc/dibbler/server.conf"
#define RELCONF_FILE       "/etc/dibbler/relay.conf"
#define RESOLVCONF_FILE    "/etc/resolv.conf"
#define NTPCONF_FILE       "/etc/ntp.conf"
#define RADVD_FILE         "/etc/dibbler/radvd.conf"
#define CLNTPID_FILE       "/var/lib/dibbler/client.pid"
#define SRVPID_FILE        "/var/lib/dibbler/server.pid"
#define RELPID_FILE        "/var/lib/dibbler/relay.pid"
#define CLNTLOG_FILE       "/var/log/dibbler/dibbler-client.log"
#define SRVLOG_FILE        "/var/log/dibbler/dibbler-server.log"
#define RELLOG_FILE        "/var/log/dibbler/dibbler-relay.log"
#define NULLFILE           "/dev/null"
#endif

/* --- options --- */
#define OPTION_DNS_SERVERS_FILENAME  "option-dns-servers"
#define OPTION_DOMAINS_FILENAME      "option-domains"
#define OPTION_NTP_SERVERS_FILENAME  "option-ntp-servers"
#define OPTION_TIMEZONE_FILENAME     "option-timezone"
#define OPTION_SIP_SERVERS_FILENAME  "option-sip-servers"
#define OPTION_SIP_DOMAINS_FILENAME  "option-sip-domains"
#define OPTION_NIS_SERVERS_FILENAME  "option-nis-servers"
#define OPTION_NIS_DOMAIN_FILENAME   "option-nis-domain"
#define OPTION_NISP_SERVERS_FILENAME "option-nisplus-servers"
#define OPTION_NISP_DOMAIN_FILENAME  "option-nisplus-domain"

/* ********************************************************************** */
/* *** interface flags ************************************************** */
/* ********************************************************************** */
/* those flags are used to parse flags in the structure
   returned by if_list_get(). They are highly system specific. */
#define LOGLEVEL          0

#ifdef WIN32
#define IF_RUNNING        0x1
#define IF_UP             0x1
#define IF_MULTICAST      0x4
#define IF_LOOPBACK       0x8
#endif

#ifdef LINUX
#define IF_UP		      0x1
#define IF_LOOPBACK	      0x8
#define IF_RUNNING	      0x40
#define IF_MULTICAST	      0x1000
#endif

#ifdef MACOS
/* dumb guesses for now. if_list_get() must be implemented for real */
#define IF_UP        0x1
#define IF_LOOPBACK  0x2
#define IF_RUNNING   0x4
#define IF_MULTICAST 0x8 
#endif

/* ********************************************************************** */
/* *** low-level error codes ******************************************** */
/* ********************************************************************** */
#define LOWLEVEL_NO_ERROR                0
#define LOWLEVEL_ERROR_UNSPEC           -1
#define LOWLEVEL_ERROR_BIND_IFACE       -2
#define LOWLEVEL_ERROR_BIND_FAILED      -4
#define LOWLEVEL_ERROR_MCAST_HOPS       -5
#define LOWLEVEL_ERROR_MCAST_MEMBERSHIP -6
#define LOWLEVEL_ERROR_GETADDRINFO      -7
#define LOWLEVEL_ERROR_SOCK_OPTS        -8
#define LOWLEVEL_ERROR_REUSE_FAILED     -9
#define LOWLEVEL_ERROR_FILE             -10
#define LOWLEVEL_ERROR_SOCKET           -11
#define LOWLEVEL_ERROR_NOT_IMPLEMENTED  -12

#define LOWLEVEL_TENTATIVE_YES 1
#define LOWLEVEL_TENTATIVE_NO  0
#define LOWLEVEL_TENTATIVE_DONT_KNOW -1

/* ********************************************************************** */
/* *** time related functions ******************************************* */
/* ********************************************************************** */

#ifdef WIN32
#define strncasecmp _strnicmp
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#endif

#define now() (unsigned long) time(NULL)

/* ********************************************************************** */
/* *** interface/socket low level functions ***************************** */
/* ********************************************************************** */

#ifdef __cplusplus 
extern "C" {
#endif

    int lowlevelInit();
    int lowlevelExit();

    /* get list of interfaces */
    extern struct iface * if_list_get();
    extern void if_list_release(struct iface * list);

    /* add address to interface */
    extern int ipaddr_add(const char* ifacename, int ifindex, const char* addr, 
			  unsigned long pref, unsigned long valid, int prefixLength);
    extern int ipaddr_update(const char* ifacename, int ifindex, const char* addr,
			     unsigned long pref, unsigned long valid, int prefixLength);
    extern int ipaddr_del(const char* ifacename, int ifindex, const char* addr, int prefixLength);
    
    /* add socket to interface */
    extern int sock_add(char* ifacename,int ifaceid, char* addr, int port, int thisifaceonly, int reuse);
    extern int sock_del(int fd);
    extern int sock_send(int fd, char* addr, char* buf, int buflen, int port, int iface);
    extern int sock_recv(int fd, char* myPlainAddr, char* peerPlainAddr, char* buf, int buflen);
    
    /* pack/unpack address */
    extern void print_packed(char addr[]);
    extern int inet_pton6(const char* src, char* dst);
    extern char * inet_ntop4(const char* src, char* dst);
    extern char * inet_ntop6(const char* src, char* dst);
    extern void print_packed(char * addr);
    extern void doRevDnsAddress( char * src, char *dst);
    extern void doRevDnsZoneRoot( char * src,  char * dst, int lenght);
    extern void truncatePrefixFromConfig( char * src,  char * dst, char lenght);
    extern int is_addr_tentative(char* ifacename, int iface, char* plainAddr);
    extern void link_state_change_init(volatile struct link_state_notify_t * changed_links, volatile int * notify);
    extern void link_state_change_cleanup();
    extern void microsleep(int microsecs);
    uint64_t htonll(uint64_t n);
    uint64_t ntohll(uint64_t n);
    extern char * error_message();

    /* options */
    extern int dns_add(const char* ifname, int ifindex, const char* addrPlain);
    extern int dns_del(const char* ifname, int ifindex, const char* addrPlain);
    extern int domain_add(const char* ifname, int ifindex, const char* domain);
    extern int domain_del(const char* ifname, int ifindex, const char* domain);
    extern int ntp_add(const char* ifname, int ifindex, const char* addrPlain);
    extern int ntp_del(const char* ifname, int ifindex, const char* addrPlain);
    extern int timezone_set(const char* ifname, int ifindex, const char* timezone);
    extern int timezone_del(const char* ifname, int ifindex, const char* timezone);
    extern int sipserver_add(const char* ifname, int ifindex, const char* addrPlain);
    extern int sipserver_del(const char* ifname, int ifindex, const char* addrPlain);
    extern int sipdomain_add(const char* ifname, int ifindex, const char* domain);
    extern int sipdomain_del(const char* ifname, int ifindex, const char* domain);
    extern int nisserver_add(const char* ifname, int ifindex, const char* addrPlain);
    extern int nisserver_del(const char* ifname, int ifindex, const char* addrPlain);
    extern int nisdomain_set(const char* ifname, int ifindex, const char* domain);
    extern int nisdomain_del(const char* ifname, int ifindex, const char* domain);
    
    extern int nisplusserver_add(const char* ifname, int ifindex, const char* addrPlain);
    extern int nisplusserver_del(const char* ifname, int ifindex, const char* addrPlain);
    extern int nisplusdomain_set(const char* ifname, int ifindex, const char* domain);
    extern int nisplusdomain_del(const char* ifname, int ifindex, const char* domain);

    extern int prefix_forwarding_enabled();
    extern int prefix_add(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
			  unsigned long prefered, unsigned long valid);
    extern int prefix_update(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
			     unsigned long prefered, unsigned long valid);
    extern int prefix_del(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength);

    char * getAAAKey(uint32_t SPI, uint32_t *len); /* reads AAA key from a file */
    char * getAAAKeyFilename(uint32_t SPI); /* which file? use this function to find out */
    uint32_t getAAASPIfromFile();
    
#ifdef __cplusplus
}
#endif

#ifdef LINUX
#ifdef DEBUG
void print_trace(void);
#endif
#endif


#endif
