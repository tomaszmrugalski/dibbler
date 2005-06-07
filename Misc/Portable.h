/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: Portable.h,v 1.43 2005-06-07 21:59:24 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.42  2005/03/15 00:36:22  thomson
 * 0.4.0 release (win32 commit)
 *
 * Revision 1.41  2005/03/08 00:43:48  thomson
 * 0.4.0-RC2 release.
 *
 * Revision 1.40  2005/02/03 19:10:26  thomson
 * *** empty log message ***
 *
 * Revision 1.39  2005/02/01 23:19:43  thomson
 * 0.4.0-RC1 version.
 *
 * Revision 1.38  2005/01/31 18:46:53  thomson
 * Win32 project fixes.
 *
 * Revision 1.37  2005/01/30 22:53:28  thomson
 * *** empty log message ***
 *
 * Revision 1.36  2005/01/25 21:14:14  thomson
 * *** empty log message ***
 *
 * Revision 1.35  2005/01/24 00:42:57  thomson
 * no message
 *
 * Revision 1.34  2005/01/23 23:17:53  thomson
 * Relay/global address support related improvements.
 *
 * Revision 1.33  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 * Revision 1.32  2004/12/27 20:45:00  thomson
 * 0.3.1 version.
 *
 * Revision 1.31  2004/12/15 22:54:45  thomson
 * no message
 *
 * Revision 1.30  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.29  2004/12/04 23:45:40  thomson
 * Problem with client and server on the same Linux host fixed (bug #56)
 *
 * Revision 1.28  2004/11/15 20:59:12  thomson
 * 0.3.0-RC2
 *
 * Revision 1.27  2004/11/01 23:31:25  thomson
 * New options,option handling mechanism and option renewal implemented.
 *
 * Revision 1.26  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.25  2004/10/03 21:53:15  thomson
 * strncasecmp macro added (win32)
 *
 * Revision 1.24  2004/10/03 21:28:45  thomson
 * 0.2.1-RC1 version.
 *
 * Revision 1.23  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.21  2004/07/11 14:09:11  thomson
 * 0.2.0-RC2
 *
 * Revision 1.18  2004/06/04 19:03:47  thomson
 * Resolved warnings with signed/unisigned
 *
 * Revision 1.17  2004/05/23 23:45:45  thomson
 * logLevel works (finally...)
 *
 * Revision 1.15  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 */

#ifndef PORTABLE_H
#define PORTABLE_H

#define DIBBLER_VERSION "0.4.1-CVS (" __DATE__ " " __TIME__ ")"
//#define DIBBLER_VERSION "0.4.1"

#define DIBBLER_COPYRIGHT1 "| Dibbler - a portable DHCPv6, version " DIBBLER_VERSION
#define DIBBLER_COPYRIGHT2 "| Authors : Tomasz Mrugalski<thomson(at)klub.com.pl>,Marek Senderski<msend(at)o2.pl>"
#define DIBBLER_COPYRIGHT3 "| Licence : GNU GPL v2 or later. Developed at Gdansk University of Technology."
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

// **********************************************************************
// *** file setup/default paths *****************************************
// **********************************************************************

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

#define RELCFGMGR_FILE    "relay-CfgMgr.xml"
#define RELIFACEMGR_FILE  "relay-IfaceMgr.xml"
#define RELTRANSMGR_FILE  "relay-TransMgr.xml"

// false - normal operation
// true - Linux: client and server can be run on the same host, but
//        there is also a drawback: multiple clients can be run at once.
#define CLIENT_BIND_REUSE true

#ifdef WIN32
#define WORKDIR           ".\\"
#define CLNTCONF_FILE	  "client.conf"
#define SRVCONF_FILE      "server.conf"
#define RELCONF_FILE      "relay.conf"
#define CLNTLOG_FILE	  "client.log"
#define SRVLOG_FILE	      "server.log"
#define RELLOG_FILE       "relay.log"
#define NULLFILE          "nul"
#endif

#ifdef LINUX
#define WORKDIR           "/var/lib/dibbler"
#define CLNTCONF_FILE     "/etc/dibbler/client.conf"
#define SRVCONF_FILE      "/etc/dibbler/server.conf"
#define RELCONF_FILE      "/etc/dibbler/relay.conf"
#define RESOLVCONF_FILE   "/etc/resolv.conf"
#define CLNTPID_FILE      "/var/lib/dibbler/client.pid"
#define SRVPID_FILE       "/var/lib/dibbler/server.pid"
#define RELPID_FILE       "/var/lib/dibbler/relay.pid"
#define CLNTLOG_FILE      "/var/lib/dibbler/dibbler-client.log"
#define SRVLOG_FILE       "/var/lib/dibbler/dibbler-server.log"
#define RELLOG_FILE       "/var/lib/dibbler/dibbler-relay.log"
#define NULLFILE          "/dev/null"
#endif

// --- options ---
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

// **********************************************************************
// *** interface flags **************************************************
// **********************************************************************

#ifdef WIN32
#define MAX_IFNAME_LENGTH 255
#define LOGLEVEL	0
#define IF_RUNNING	0x1
#define IF_UP		0x1
#define IF_MULTICAST	0x4
#define IF_LOOPBACK	0x8
#endif

#ifdef LINUX
#define MAX_IFNAME_LENGTH 64
#define LOGLEVEL	0
#define IF_UP		0x1
#define IF_LOOPBACK	0x8  
#define IF_RUNNING	0x40
#define IF_MULTICAST	0x1000
#endif

// **********************************************************************
// *** time related functions *******************************************
// **********************************************************************

#ifdef WIN32
#define strncasecmp _strnicmp
#include <windows.h>
#include <time.h>
#endif

#define now() (unsigned) time(NULL)

// **********************************************************************
// *** interface/socket low level functions *****************************
// **********************************************************************

#ifdef __cplusplus 
extern "C" {
#endif

extern struct iface * if_list_get();
extern void if_list_release(struct iface * list);
    
/* add address to interface */
extern int ipaddr_add(const char* ifacename, int ifindex, const char* addr, 
		      unsigned long pref, unsigned long valid);
extern int ipaddr_del(const char* ifacename, int ifindex, const char* addr);

/* add socket to interface */
extern int sock_add(char* ifacename,int ifaceid, char* addr, int port, int thisifaceonly, int reuse);
extern int sock_del(int fd);
extern int sock_send(int fd, char* addr, char* buf, int buflen, int port, int iface);
extern int sock_recv(int fd, char* myPlainAddr, char* peerPlainAddr, char* buf, int buflen);

/* pack/unpack address */
extern void print_packed(char addr[]);
extern int inet_pton4(const char* src, char* dst);
extern int inet_pton6(const char* src, char* dst);
extern char * inet_ntop4(const char* src, char* dst);
extern char * inet_ntop6(const char* src, char* dst);
extern void print_packed(char * addr);
extern int is_addr_tentative(char* ifacename, int iface, char* plainAddr);
/* microsleep(int microsecs) */
extern void microsleep(int microsecs);

/* DNS */
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

#ifdef __cplusplus
}
#endif

#ifndef MAX_IFNAME_LENGTH 
#define MAX_IFNAME_LENGTH 255
#endif

struct iface {
    char name[MAX_IFNAME_LENGTH];  // interface name
    int  id;                       // interface ID (often called ifindex)
    int  hardwareType;             // type of hardware (see RFC 826)
    char mac[255];                 // link layer address
    int  maclen;                   // length of link layer address
    char *linkaddr;                // assigned IPv6 link local addresses 
    int  linkaddrcount;            // number of assigned IPv6 link local addresses
    char *globaladdr;              // global IPv6 addresses
    int  globaladdrcount;          // number of global IPV6 addresses
    unsigned int flags;            // look IF_xxx in portable.h
    struct iface* next;            // structure describing next iface in system
};

#endif
