
#ifndef PORTABLE_H
#define PORTABLE_H

#define DIBBLER_VERSION "0.0.20040114"

#define DIBBLER_COPYRIGHT1 "Dibbler - a portable DHCPv6, version " DIBBLER_VERSION
#define DIBBLER_COPYRIGHT2 "by Tomasz Mrugalski <thomson@klub.com.pl> engine,Linux port,doc"
#define DIBBLER_COPYRIGHT3 "   Marek Senderski  <msend@o2.pl>         engine,winXP port"
#define DIBBLER_COPYRIGHT4 "Published under GNU GPL v2 or later licence. "

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

#ifdef WIN32
#define WORKDIR		   ".\\"
#define SRVDUID_FILE	   "duid-server"
#define CLNTDUID_FILE	   "duid-client"
#define CLNTLOG_FILE	   "client.log"
#define SRVLOG_FILE	   "server.log"
#define CLNTCONF_FILE	   "client.conf"
#define SRVCONF_FILE       "server.conf"
#define CLNTDB_FILE	   "addrdb-client.xml"
#define SRVDB_FILE         "addrdb-server.xml"
#endif

#ifdef LINUX
#define CLNTCONF_FILE	   "/var/lib/dibbler/client.conf"
#define SRVCONF_FILE       "/var/lib/dibbler/server.conf"
#define WORKDIR		   "/var/lib/dibbler"
#define SRVDUID_FILE	   "duid-server"
#define CLNTDUID_FILE	   "duid-client"
#define CLNTLOG_FILE	   "client.log"
#define SRVLOG_FILE	   "server.log"
#define RESOLVCONF_FILE    "resolv.conf"
#define CLNTDB_FILE	   "addrdb-client.xml"
#define SRVDB_FILE         "addrdb-server.xml"
#define CLNTPID_FILE       "client.pid"
#define SRVPID_FILE        "server.pid"
#endif

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
#include <windows.h>
#include <time.h>
extern time_t timer;
#define now() time(&timer)
#endif

#ifdef LINUX
#define now() time(NULL)
#endif

// **********************************************************************
// *** interface/socket low level functions *****************************
// **********************************************************************

#ifdef __cplusplus 
extern "C" {
#endif

extern struct iface * if_list_get();
extern void if_list_release(struct iface * list);
    
/* add address to interface */
extern int ipaddr_add(char* ifacename, int ifaceid, char* addr, long pref, long valid);
extern int ipaddr_del(char* ifacename, int ifaceid, char* addr);

/* add socket to interface */
extern int sock_add(char* ifacename,int ifaceid, char* addr, int port, int thisifaceonly);
extern int sock_del(int fd);
extern int sock_send(int fd, char* addr, char* buf, int buflen, int port, int iface);
extern int sock_recv(int fd, char* addr, char* buf, int buflen);

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
extern int dns_add(const char* ifname, int ifaceid, char* addrPlain);
extern int dns_del(const char* ifname, int ifaceid, char* addrPlain);
extern int domain_add(const char* ifname, int ifaceid, char* domain);
extern int domain_del(const char* ifname, int ifaceid, char* domain);

#ifdef __cplusplus
}
#endif

#ifndef MAX_IFNAME_LENGTH 
#define MAX_IFNAME_LENGTH 255
#endif

struct iface {
    char name[MAX_IFNAME_LENGTH];   //name of interface
    int  id;                        //interface id
    int  hardwareType;  //type of hardware (RFC 826)
    char mac[255];      //link layer address
    int  maclen;        //length of link layer address
    char *linkaddr;   //assigned IPv6 link local addresses 
    int  linkaddrcount; //number of assigned IPv6 link local addresses
    unsigned int flags;//look IF_xxx in portable.h
    struct iface* next; //structure describing next iface in system
};

#endif
