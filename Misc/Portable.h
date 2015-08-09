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
 */

#ifndef PORTABLE_H
#define PORTABLE_H

#define DIBBLER_VERSION "1.0.1"

#define DIBBLER_COPYRIGHT1 "| Dibbler - a portable DHCPv6, version " DIBBLER_VERSION
#define DIBBLER_COPYRIGHT2 "| Authors : Tomasz Mrugalski<thomson(at)klub.com.pl>,Marek Senderski<msend(at)o2.pl>"
#define DIBBLER_COPYRIGHT3 "| Licence : GNU GPL v2 only. Developed at Gdansk University of Technology."
#define DIBBLER_COPYRIGHT4 "| Homepage: http://klub.com.pl/dhcpv6/"

#ifdef WIN32
#include <winsock2.h>
#define strcasecmp strcmpi
#define snprintf _snprintf
#endif
#if defined(LINUX) || defined(BSD)
#include <arpa/inet.h>
#endif

#include <stdint.h>
#include <unistd.h>

/* this should look like this: 
uint16_t readUint16(uint8_t* buf);
uint8_t * writeUint16(uint8_t* buf, uint16_t word);
uint32_t readUint32(uint8_t* buf);
uint8_t* writeUint32(uint8_t* buf, uint32_t dword);
uint64_t readUint64(uint8_t* buf);
uint8_t* writeUint64(uint8_t* buf, uint64_t qword); */

/* due to poor type usage (char* instead of uint8_t*), we need to stick with
   char* for now. Changing to uint8_t would require rewriting large parts of the code */
#define BUFFER_TYPE char

#ifdef __cplusplus 
extern "C" {
#endif
    uint8_t readUint8(const BUFFER_TYPE* buf);
    BUFFER_TYPE* writeUint8(BUFFER_TYPE* buf, uint8_t octet);
    uint16_t readUint16(const BUFFER_TYPE * buf);
    BUFFER_TYPE * writeUint16(BUFFER_TYPE * buf, uint16_t word);
    uint32_t readUint32(const BUFFER_TYPE * buf);
    BUFFER_TYPE * writeUint32(BUFFER_TYPE * buf, uint32_t dword);
    uint64_t readUint64(const BUFFER_TYPE * buf);
    BUFFER_TYPE * writeUint64(BUFFER_TYPE * buf, uint64_t qword);
    BUFFER_TYPE* writeData(BUFFER_TYPE* buf,  BUFFER_TYPE* data, size_t len);
#ifdef __cplusplus 
}
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
    int  link_state;               /* used in link change detection routines */
    unsigned int flags;            /* look IF_xxx in portable.h */
    unsigned char m_bit;           /* M bit in RA received? */
    unsigned char o_bit;           /* O bit in RA received? */
    struct iface* next;            /* structure describing next iface in system */
};

#define MAX_LINK_STATE_CHANGES_AT_ONCE 16

struct link_state_notify_t
{
    int ifindex[MAX_LINK_STATE_CHANGES_AT_ONCE]; /* indexes of interfaces that has changed. 
                                                    Only non-zero values will be used */
    int stat[MAX_LINK_STATE_CHANGES_AT_ONCE];
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
#define DNSUPDATE_DEFAULT_TIMEOUT 1000 /* in ms */
#define INACTIVE_MODE_INTERVAL 3 /* 3 seconds */

#define REQLOG_FILE        "dibbler-requestor.log"

#ifdef WIN32
#define DEFAULT_WORKDIR       ".\\"
#define DEFAULT_SCRIPT        ""
#define DEFAULT_CLNTCONF_FILE "client.conf"
#define SRVCONF_FILE          "server.conf"
#define RELCONF_FILE          "relay.conf"
#define DEFAULT_CLNTLOG_FILE  "dibbler-client.log"
#define SRVLOG_FILE           "dibbler-server.log"
#define RELLOG_FILE           "dibbler-relay.log"
#define CLNT_AAASPI_FILE      "AAA-SPI"
#define SRV_KEYMAP_FILE       "keys-mapping"
#define NULLFILE              "nul"
/* specifies if client should remove any configured DNS servers when configuring
   DNS servers for the first time. This makes sense on WIN32 only. */
#define FLUSH_OTHER_CONFIGURED_DNS_SERVERS true
#endif

#if defined(LINUX) || defined(BSD) || defined(SUNOS)

#define DEFAULT_WORKDIR       "/var/lib/dibbler"
#define DEFAULT_CLNTCONF_FILE "/etc/dibbler/client.conf"
#define DEFAULT_CLNTPID_FILE  "/var/lib/dibbler/client.pid"
#define DEFAULT_CLNTLOG_FILE  "/var/log/dibbler/dibbler-client.log"

#define DEFAULT_SCRIPT     ""
#define SRVCONF_FILE       "/etc/dibbler/server.conf"
#define RELCONF_FILE       "/etc/dibbler/relay.conf"
#define RESOLVCONF_FILE    "/etc/resolv.conf"
#define NTPCONF_FILE       "/etc/ntp.conf"
#define RADVD_FILE         "/etc/dibbler/radvd.conf"
#define SRVPID_FILE        "/var/lib/dibbler/server.pid"
#define RELPID_FILE        "/var/lib/dibbler/relay.pid"
#define SRVLOG_FILE        "/var/log/dibbler/dibbler-server.log"
#define RELLOG_FILE        "/var/log/dibbler/dibbler-relay.log"
#define CLNT_AAASPI_FILE   "/var/lib/dibbler/AAA/AAA-SPI"
#define SRV_KEYMAP_FILE    "/var/lib/dibbler/AAA/keys-mapping"
#define NULLFILE           "/dev/null"

/* those defines were initially used on Linux only, but hopefully 
   they will work on other Posix systems as well */
#define RESOLVCONF         "/sbin/resolvconf"
#define TIMEZONE_FILE      "/etc/localtime"
#define TIMEZONES_DIR      "/usr/share/zoneinfo"

/* specifies if client should remove any configured DNS servers when configuring
   DNS servers for the first time. This makes sense on WIN32 only. */
#define FLUSH_OTHER_CONFIGURED_DNS_SERVERS false
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

/* --- bulk leasequery --- */
#define BULKLQ_ACCEPT false
#define BULKLQ_TCP_PORT 547
#define BULKLQ_MAX_CONNS 10
#define BULKLQ_TIMEOUT 300

/* ********************************************************************** */
/* *** interface flags ************************************************** */
/* ********************************************************************** */
/* those flags are used to parse flags in the structure
   returned by if_list_get(). They are highly system specific.
   Posix systems use values imported from headers */

#ifdef WIN32
#define IFF_RUNNING        IFF_UP
/* those defines are in ws2ipdef.h
#define IFF_UP             0x1
#define IFF_MULTICAST      0x4
#define IFF_LOOPBACK       0x8 */
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

    /** @brief gets MAC address from the specified IPv6 address
     *
     *  This is called immediately after we received message from that address,
     *  so the neighbor cache is guaranteed to have entry for it. This is highly
     *  os specific
     *
     * @param iface_name network interface name
     * @param ifindex network interface index
     * @param v6addr text represenation of the IPv6 address
     * @param mac pointer to MAC buffer (length specified in mac_len)
     * @param mac_len specifies MAC buffer length. Must be set so number of bytes set by the function
     *
     * @return 0 if successful, negative values if there are errors
     */
    extern int get_mac_from_ipv6(const char* iface_name, int ifindex, const char* v6addr,
                                 char* mac, int* mac_len);
    
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
    extern void link_state_change_init(volatile struct link_state_notify_t * changed_links,
                                       volatile int * notify);
    extern void link_state_change_cleanup();
    extern void microsleep(int microsecs);
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

    extern int prefix_add(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
			  unsigned long prefered, unsigned long valid);
    extern int prefix_update(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
			     unsigned long prefered, unsigned long valid);
    extern int prefix_del(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength);

    char * getAAAKey(uint32_t SPI, uint32_t *len); /* reads AAA key from a file */
    char * getAAAKeyFilename(uint32_t SPI); /* which file? use this function to find out */
    uint32_t getAAASPIfromFile();

    int execute(const char *filename, const char * argv[], const char *env[]);

    /** @brief fills specified buffer with random data
     * @param buffer random data will be written here
     * @param len length of the buffer
     */
    void fill_random(uint8_t* buffer, int len);

    /** @brief returns host name of this host
     *
     * @param hostname buffer (hostname will be stored here)
     * @param hostname_len length of the buffer
     * @return LOWLEVEL_NO_ERROR if successful, appropriate LOWLEVEL_ERROR_* otherwise
     */
    int get_hostname(char* hostname, int hostname_len);
    
#ifdef __cplusplus
}
#endif

#ifdef LINUX
#ifdef DEBUG
void print_trace(void);
#endif
#endif


#endif
