/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: lowlevel-win32.c,v 1.22 2008-10-13 22:41:18 thomson Exp $
 *
 */

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <Ws2tcpip.h.>
#include <Ws2spi.h>

#include <iphlpapi.h>
#include <iptypes.h>
#include <process.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#include "Portable.h"
#include "DHCPConst.h"

char netshPath[256];
char cmdPath[256];


#define ERROR_MESSAGE_SIZE 1024
static char Message[ERROR_MESSAGE_SIZE] = {0};

static void error_message_set(int errCode);
static void error_message_set_string(char *str);

/* 
  Find netsh.exe
 */
int lowlevelInit()
{
    char buf[256];
    FILE *f;
    int i;
    i = GetEnvironmentVariable("SYSTEMROOT",buf, 256);
    if (!i) {
        error_message_set_string("Environment variable SYSTEMROOT not set.\n");
	    return 0;
    }
    strcpy(buf+i,"\\system32\\netsh.exe");
    if (!(f=fopen(buf,"r"))) {
        sprintf(Message, "Unable to open %s file.\n",buf);
	    return 0;
    }
    fclose(f);
    memcpy(netshPath, buf,256);
    strcpy(buf+i,"\\system32\\cmd.exe");
    memcpy(cmdPath, buf,256);
    return 1;
}

/* when updating this file, remember to also update copy in Port-winnt2k/lowlevel-winnt2k.c */
uint32_t getAAASPIfromFile() {
    char filename[1024];
    struct stat st;
    uint32_t ret;
    FILE *file;

    strcpy(filename, "AAA-SPI");

    if (stat(filename, &st))
        return 0;

    file = fopen(filename, "r");
    if (!file)
        return 0;

    fscanf(file, "%x", &ret);
    fclose(file);

    return ret;
}

/* when updating this file, remember to also update copy in Port-winnt2k/lowlevel-winnt2k.c */
char * getAAAKeyFilename(uint32_t SPI)
{
    static char filename[1024];
    if (SPI)
        snprintf(filename, 1024, "%s%s%x", "", "AAA-key-", SPI);
    else
        strcpy(filename, "AAA-key");
    return filename;
}

/* when updating this file, remember to also update copy in Port-winnt2k/lowlevel-winnt2k.c */
char * getAAAKey(uint32_t SPI, uint32_t *len) {

    char * filename;
    struct stat st;
    char * retval;
    int offset = 0;
    int fd;
    int ret;

    filename = getAAAKeyFilename(SPI);

    if (stat(filename, &st))
        return NULL;

    fd = open(filename, O_RDONLY);
    if (0 > fd)
        return NULL;

    /** @todo should be freed somewhere */
    retval = malloc(st.st_size);
    if (!retval)
        return NULL;

    while (offset < st.st_size) {
        ret = read(fd, retval + offset, st.st_size - offset);
        if (!ret) break;
        if (ret < 0) {
            return NULL;
        }
        offset += ret;
    }
    close(fd);

    if (offset != st.st_size)
        return NULL;

    *len = st.st_size;
    return retval;
}

char * error_message()
{
	return Message;
}

void error_message_set(int errCode) {
	char tmp[ERROR_MESSAGE_SIZE-10];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)tmp, ERROR_MESSAGE_SIZE-10, NULL);
	sprintf(Message, "Error %d: %s\n", errCode, tmp);
}

void error_message_set_string(char *str) {
    strncpy(Message, str, ERROR_MESSAGE_SIZE);
}

void if_list_release(struct iface * list) {
    struct iface * tmp;
    while (list) {
        tmp = list->next;
	if (list->linkaddrcount)
	    free(list->linkaddr);
        free(list);
        list = tmp;
    }
}

extern	struct iface* if_list_get()
{
    char* buffer=NULL;
    unsigned long buflen;
    struct  iface* iface;
    struct  iface* retval;
    struct  sockaddr_in6 *addrpck;
    char*  addr;
    char * gaddr;
    int linkLocalAddrCnt;
    int globalAddrCnt;
    PIP_ADAPTER_ADDRESSES adaptaddr;
    PIP_ADAPTER_UNICAST_ADDRESS linkaddr;

    buflen=0;
    GetAdaptersAddresses(AF_INET6, GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST|GAA_FLAG_SKIP_DNS_SERVER,
                         NULL, (PIP_ADAPTER_ADDRESSES) buffer, &buflen);
    
    if (!buflen) {
        // no interfaces found. Probably IPv6 is not installed.
        return NULL;
    }
    
    buffer=(char*)malloc(buflen);
    
    GetAdaptersAddresses(AF_INET6,GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST
			 |GAA_FLAG_SKIP_DNS_SERVER,NULL,(PIP_ADAPTER_ADDRESSES) buffer,&buflen);
    adaptaddr=(PIP_ADAPTER_ADDRESSES) buffer;
    
    retval=iface=(struct iface*) malloc(sizeof(struct iface));
    while(adaptaddr){
    	//set interface name and id
    	WideCharToMultiByte( CP_ACP, 0, adaptaddr->FriendlyName, -1,
				  		     iface->name,MAX_IFNAME_LENGTH, NULL, NULL );
        iface->id=adaptaddr->IfIndex;
        iface->id=adaptaddr->Ipv6IfIndex;
        
        //set hardware type of interface		
        iface->hardwareType=adaptaddr->IfType;
	
        //set physical address - require during DUID generation
        memcpy(iface->mac,adaptaddr->PhysicalAddress,adaptaddr->PhysicalAddressLength);
	
        //set link local addresses available on interface
        iface->maclen=adaptaddr->PhysicalAddressLength;

        iface->globaladdrcount = 0;
        iface->globaladdr = 0;
	
        linkaddr=adaptaddr->FirstUnicastAddress;
        //for evert unicast address on iface
        linkLocalAddrCnt= 0;
	    globalAddrCnt   = 0;
	    while(linkaddr) {
	        addrpck=(struct sockaddr_in6*)linkaddr->Address.lpSockaddr;
    		if (IN6_IS_ADDR_LINKLOCAL(( (struct in6_addr*) (&addrpck->sin6_addr)))) {
                linkLocalAddrCnt++;
		    } else {
    		    globalAddrCnt++;
	    	}
            linkaddr=linkaddr->Next;
        }
        
        iface->linkaddrcount   = linkLocalAddrCnt;
    	iface->globaladdrcount = globalAddrCnt;
    	iface->linkaddr        = NULL;
    	iface->globaladdr      = NULL;
    	iface->linkaddr        = (char*) malloc(linkLocalAddrCnt*16);
        iface->globaladdr      = (char*) malloc(globalAddrCnt*16);
        addr = iface->linkaddr;
        gaddr= iface->globaladdr;
            
        linkaddr=adaptaddr->FirstUnicastAddress;
        while(linkaddr) {
            addrpck=(struct sockaddr_in6*)linkaddr->Address.lpSockaddr;
     	    if (IN6_IS_ADDR_LINKLOCAL(( (struct in6_addr*) (&addrpck->sin6_addr)))) {
                memcpy(addr,&(addrpck->sin6_addr),16);
                addr+=16;
            } else {
    		    memcpy(gaddr, &(addrpck->sin6_addr), 16);
    		    gaddr+=16;
    		}
            linkaddr=linkaddr->Next;
        }

    	//set interface flags
    	iface->flags=0;
    	if (adaptaddr->OperStatus==IfOperStatusUp)
	        iface->flags|=IF_UP|IF_RUNNING|IF_MULTICAST;
	    if (adaptaddr->IfType==IF_TYPE_SOFTWARE_LOOPBACK)
    	    iface->flags|=IF_LOOPBACK;
	
    	//go to next returned adapter
    	if (adaptaddr->Next)
	        iface->next=(struct iface*) malloc(sizeof(struct iface));
	    else
    	    iface->next=NULL;

		adaptaddr=adaptaddr->Next;
    	iface=iface->next;
    }	
    free(buffer);
    return retval;
}

extern int is_addr_tentative(char* ifacename, int iface, char* plainAddr)
{
    char* buffer=NULL;
    struct  sockaddr_in6 *addrpck;
    PIP_ADAPTER_ADDRESSES adaptaddr;
    PIP_ADAPTER_UNICAST_ADDRESS linkaddr;
    char netAddr[16];
    unsigned long buflen=0;
    int retVal=-1;
    
    PIP_ADAPTER_UNICAST_ADDRESS found=NULL;
    
    inet_pton6(plainAddr,netAddr);
    GetAdaptersAddresses(AF_INET6,GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST
			 |GAA_FLAG_SKIP_DNS_SERVER,NULL,(PIP_ADAPTER_ADDRESSES) buffer,&buflen);
    buffer=(char*)malloc(buflen);
    GetAdaptersAddresses(AF_INET6,GAA_FLAG_SKIP_ANYCAST|GAA_FLAG_SKIP_MULTICAST
			 |GAA_FLAG_SKIP_DNS_SERVER,NULL,(PIP_ADAPTER_ADDRESSES) buffer,&buflen);
    adaptaddr=(PIP_ADAPTER_ADDRESSES) buffer;
    
    while(adaptaddr&&(!found))	
    {
	    linkaddr=adaptaddr->FirstUnicastAddress;
	    //for evert unicast address on iface
	    while(linkaddr&&(!found))
	    {
	        addrpck=(struct sockaddr_in6*)linkaddr->Address.lpSockaddr;
	        if (!memcmp((struct in6_addr*)(&addrpck->sin6_addr),netAddr,16))
		        found=linkaddr;
	        linkaddr=linkaddr->Next;
	    }
	    adaptaddr=adaptaddr->Next;
    }
    
    free(buffer);
    if (!found)
        return TENTATIVE_UNKNOWN; /* not found */
    if (found->DadState==IpDadStateDuplicate)
        return TENTATIVE_YES;     /* tentative */
    else
        return TENTATIVE_NO;      /* not tentative */
}
extern int ipaddr_add(const char * ifacename, int ifaceid, const char * addr, 
                      unsigned long pref, unsigned long valid, int prefixLen)
{
    // netsh interface ipv6 add address interface="eth0" address=2000::123 validlifetime=120 preferredlifetime=60
    char arg1[]="interface";
    char arg2[]="ipv6";
    char arg3[]="add";
    char arg4[]="address";
    char arg5[256]; // interface=...
    char arg6[256]; // address=...
    char arg7[256]; // valid
    char arg8[256]; // pref
    intptr_t i;
    sprintf(arg5,"interface=\"%s\"", ifacename);
    sprintf(arg6,"address=%s", addr);
    sprintf(arg7,"validlifetime=%d", valid);
    sprintf(arg8,"preferredlifetime=%d", pref);
    // use _P_DETACH to speed things up, (but the tentative detection will surely fail)
    i=_spawnl(_P_WAIT, netshPath, netshPath, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, NULL);
    return i;
}

int ipaddr_update(const char* ifacename, int ifindex, const char* addr,
		  unsigned long pref, unsigned long valid, int prefixLength)
{
    return ipaddr_add(ifacename, ifindex, addr, pref, valid, prefixLength);
}

int ipaddr_del(const char * ifacename, int ifaceid, const char * addr, int prefixLength)
{
    // netsh interface ipv6 add address interface=eth0 address=2000::123 validlifetime=120 preferredlifetime=60
    char arg1[]="interface";
    char arg2[]="ipv6";
    char arg3[]="delete";
    char arg4[]="address";
    char arg5[256]; // interface=...
    char arg6[256]; // address=...
    intptr_t i;
    sprintf(arg5,"interface=\"%s\"", ifacename);
    sprintf(arg6,"address=%s", addr);
    i=_spawnl(_P_WAIT, netshPath, netshPath, arg1, arg2, arg3, arg4, arg5, arg6, NULL);

    return i;
}

SOCKET mcast=0;

extern int sock_add(char * ifacename,int ifaceid, char * addr, int port, int thisifaceonly, int reuse)
{
    SOCKET s;
    struct sockaddr_in6 bindme;
    struct ipv6_mreq ipmreq; 
    int	hops=1;
    char packedAddr[16];
    char multiAddr[16] = { 0xff,2, 0,0, 0,0, 0,0, 0,0, 0,0, 0,1, 0,2};
    
    inet_pton6(addr,packedAddr);
    if ((s=socket(AF_INET6,SOCK_DGRAM, 0)) == INVALID_SOCKET)
	return -2;
    
    memset(&bindme, 0, sizeof(bindme));
    bindme.sin6_family   = AF_INET6;
    bindme.sin6_port     = htons(port);
    
    if (IN6_IS_ADDR_LINKLOCAL((IN6_ADDR*)packedAddr))
	bindme.sin6_scope_id = ifaceid;
    
    if (!IN6_IS_ADDR_MULTICAST((IN6_ADDR*)packedAddr)) 	{
        inet_pton6(addr, (char*)&bindme.sin6_addr);
    } 
    
    // REUSEADDR must be before bind() in order to take effect
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&hops, sizeof(hops))) {
		error_message_set(WSAGetLastError());
        return LOWLEVEL_ERROR_REUSE_FAILED;
	}

    if (bind(s, (struct sockaddr*)&bindme, sizeof(bindme))) {
        error_message_set(WSAGetLastError());
        return LOWLEVEL_ERROR_BIND_FAILED;
    }
    
    if (IN6_IS_ADDR_MULTICAST((IN6_ADDR*)packedAddr)) {
        /* multicast */
        ipmreq.ipv6mr_interface=ifaceid;
        memcpy(&ipmreq.ipv6mr_multiaddr,packedAddr,16);
        if(setsockopt(s,IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,(char*)&ipmreq,sizeof(ipmreq))) {
            error_message_set(WSAGetLastError());
            return LOWLEVEL_ERROR_MCAST_MEMBERSHIP;
        }
        if(setsockopt(s,IPPROTO_IPV6,IPV6_MULTICAST_HOPS,(char*)&hops,sizeof(hops))) {
            error_message_set(WSAGetLastError());
	        return LOWLEVEL_ERROR_MCAST_HOPS;
        }
    } 
    return s;
}

int sock_del(int fd)
{
    return closesocket(fd);
}
int sock_send(int fd, char * addr, char * buf, int buflen, int port,int iface)
{	
    ADDRINFO inforemote,*remote;
    char addrStr[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")+5];
    char portStr[10];
    int i;
    char packaddr[16];
    char ifaceStr[10];
    
    strcpy(addrStr,addr);
    itoa(port,portStr,10);
    itoa(iface,ifaceStr,10);
    inet_pton6(addrStr,packaddr);
    
    if(IN6_IS_ADDR_LINKLOCAL((struct in6_addr*)packaddr)
       ||IN6_IS_ADDR_SITELOCAL((struct in6_addr*)packaddr))
	strcat(strcat(addrStr,"%"),ifaceStr);
      
    memset(&inforemote, 0, sizeof(inforemote));
    inforemote.ai_flags=AI_NUMERICHOST;
    inforemote.ai_family=PF_INET6;
    inforemote.ai_socktype=SOCK_DGRAM;
    inforemote.ai_protocol=IPPROTO_IPV6;
    //inet_ntop6(addr,addrStr);
    if(getaddrinfo(addrStr,portStr,&inforemote,&remote))
	return 0;
    i=sendto(fd,buf,buflen,0,remote->ai_addr,remote->ai_addrlen);
	freeaddrinfo(remote);
	if (i==SOCKET_ERROR)
    {
		error_message_set(WSAGetLastError());
        return LOWLEVEL_ERROR_UNSPEC;
    }

    return LOWLEVEL_NO_ERROR;
}

int sock_recv(int fd, char * myPlainAddr, char * peerPlainAddr, char * buf, int buflen)
{
    struct sockaddr_in6 info;  
    int	infolen ;		
    int	readBytes;
    
    infolen=sizeof(info);
    if(!(readBytes=recvfrom(fd,buf,buflen,0,(SOCKADDR*)&info,&infolen))) {
        sprintf(Message, "socket reception failed (recvfrom() function returned 0 bytes read)\n");
        return LOWLEVEL_ERROR_UNSPEC;
    } else {
        inet_ntop6(info.sin6_addr.u.Byte,peerPlainAddr);
        return	readBytes;
    }
}

extern int dns_add(const char* ifname, int ifaceid, const char* addrPlain) {
    
    // netsh interface ipv6 add dns "eth0" address=2000::123
    char arg1[]="interface";
    char arg2[]="ipv6";
    char arg3[]="add";
    char arg4[]="dns";
    char arg5[256]; // interface=...
    char arg6[256]; // address=...
    intptr_t i;
    
    sprintf(arg5,"\"%s\"", ifname);
    sprintf(arg6,"address=%s", addrPlain);
    i=_spawnl(_P_DETACH,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6,NULL);

    return LOWLEVEL_NO_ERROR;
}
extern int dns_del(const char* ifname, int ifaceid, const char* addrPlain) {
    // netsh interface ipv6 add dns interface="eth0" address=2000::123
    char arg1[]="interface";
    char arg2[]="ipv6";
    char arg3[]="delete";
    char arg4[]="dns";
    char arg5[256]; // interface=...
    char arg6[256]; // address=...
    intptr_t i;
    sprintf(arg5,"\"%s\"", ifname);
    sprintf(arg6,"address=%s", addrPlain);
    i=_spawnl(_P_DETACH,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6,NULL);

    /// @todo: check status
    return LOWLEVEL_NO_ERROR;
}

extern int domain_add(const char* ifname, int ifaceid, const char* domain) {
    return LOWLEVEL_NO_ERROR;
}

extern int domain_del(const char* ifname, int ifaceid, const char* domain) {
    return LOWLEVEL_NO_ERROR;
}

int ntp_add(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int ntp_del(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int timezone_set(const char* ifname, int ifindex, const char* timezone){
    return LOWLEVEL_NO_ERROR;
}

int timezone_del(const char* ifname, int ifindex, const char* timezone){
    return LOWLEVEL_NO_ERROR;
}

int sipserver_add(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int sipserver_del(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int sipdomain_add(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int sipdomain_del(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisserver_add(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisserver_del(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisdomain_set(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisdomain_del(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusserver_add(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusserver_del(const char* ifname, int ifindex, const char* addrPlain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusdomain_set(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int nisplusdomain_del(const char* ifname, int ifindex, const char* domain){
    return LOWLEVEL_NO_ERROR;
}

int prefix_add(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
               unsigned long prefered, unsigned long valid)
{
    // netsh interface ipv6 add route 2000::/64 "eth1" preferredlifetime=1000 validlifetime=2000 store=active [publish=age]
    char arg1[]="interface";
    char arg2[]="ipv6";
    char arg3[]="add";
    char arg4[]="route";
    char arg5[256]; // prefix
    char arg6[256]; // interface=...
    char arg7[256]; // preferredlifetime=...
    char arg8[256]; // validlifetime=...
    char arg9[]="store=active";
    char arg10[]="publish=age";
    intptr_t i;
    char buf[2000];
    
    sprintf(arg5, "%s/%d", prefixPlain, prefixLength);
    sprintf(arg6,"interface=\"%s\"", ifname);
    sprintf(arg7,"preferredlifetime=%d", prefered);
    sprintf(arg8,"validlifetime=%d", valid);

    sprintf(buf, "%s %s %s %s %s %s %s %s %s %s", arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    i=_spawnl(_P_DETACH,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10, NULL);

    if (i==-1) {
        /// @todo: some better error support
        return -1;
    }

    return LOWLEVEL_NO_ERROR;
}

int prefix_update(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength,
                  unsigned long prefered, unsigned long valid)
{
    return prefix_add(ifname, ifindex, prefixPlain, prefixLength, prefered, valid);
}

int prefix_del(const char* ifname, int ifindex, const char* prefixPlain, int prefixLength)
{
    // netsh interface ipv6 del route 2000::/64 "eth1"
    char arg1[]="interface";
    char arg2[]="ipv6";
    char arg3[]="delete";
    char arg4[]="route";
    char arg5[256]; // prefix
    char arg6[256]; // interface=...
    intptr_t i;
    
    sprintf(arg5, "%s/%d", prefixPlain, prefixLength);
    sprintf(arg6,"interface=\"%s\"", ifname);
    i=_spawnl(_P_DETACH,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6, NULL);

    if (i==-1) {
        /// @todo: some better error support
        return -1;
    }

    return LOWLEVEL_NO_ERROR;
}

void link_state_change_init(volatile struct link_state_notify_t * monitored_links, volatile int * notify)
{
    /// @todo: implement this
}

void link_state_change_cleanup()
{
   /// @todo: implement this
}
