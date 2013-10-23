/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#define WIN32_LEAN_AND_MEAN

/* tells stdlib.h to include rand_s - more secure version */
#define _CRT_RAND_S
#include <winsock2.h>

/* causes compilation errors on Visual Studio 2008 because of IPAddr type being undefined */
/* commented out for now */
/* #include <icmpapi.h> */

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

    fscanf(file, "%10x", &ret);
    fclose(file);

    return ret;
}

char * getAAAKeyFilename(uint32_t SPI)
{
    static char filename[1024];
    if (SPI)
        snprintf(filename, 1024, "%s%s%x", "", "AAA-key-", SPI);
    else
        strcpy(filename, "AAA-key");
    return filename;
}

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

    retval = malloc(st.st_size);
    if (!retval)
        return NULL;

    while (offset < st.st_size) {
        ret = read(fd, retval + offset, st.st_size - offset);
        if (!ret) break;
        if (ret < 0) {
            free(retval);
            return NULL;
        }
        offset += ret;
    }
    close(fd);

    if (offset != st.st_size) {
        free(retval);
        return NULL;
    }

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
    	if (adaptaddr->OperStatus == IfOperStatusUp)
	        iface->flags |= IFF_UP|IFF_RUNNING|IFF_MULTICAST;
	    if (adaptaddr->IfType==IF_TYPE_SOFTWARE_LOOPBACK)
    	    iface->flags |= IFF_LOOPBACK;
	
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
    
	int result;
    if (!found)
        result = ADDRSTATUS_UNKNOWN; /* not found */
    else if (found->DadState==IpDadStateDuplicate)
		result = ADDRSTATUS_YES;     /* tentative */
    else
		result = ADDRSTATUS_NO;      /* not tentative */
	free(buffer);
	return result;
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
    sprintf(arg7,"validlifetime=%u", valid);
    sprintf(arg8,"preferredlifetime=%u", pref);
    // use _P_DETACH to speed things up, (but the tentative detection will surely fail)
    i=_spawnl(_P_WAIT, netshPath, netshPath, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, NULL);
    if (i) { i = _spawnl(_P_WAIT, netshPath, netshPath, arg1, arg2, "set", arg4, arg5, arg6, arg7, arg8, NULL); }
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

    memset(addrStr,  0, sizeof(addrStr));
    memset(portStr,  0, sizeof(portStr));
    memset(packaddr, 0, sizeof(packaddr));
    memset(ifaceStr, 0, sizeof(ifaceStr));
    
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
    i=_spawnl(_P_WAIT,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6,NULL);
    if (i == 0) {
        return LOWLEVEL_NO_ERROR;
    } else {
        sprintf(Message, "%s %s %s %s %s %s %s returned non-zero returncode %d",
                netshPath, arg1, arg2, arg3, arg4, arg5, arg6, i);
        return LOWLEVEL_ERROR_UNSPEC;
    }
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
    if (addrPlain)
		sprintf(arg6,"address=%s", addrPlain);
	else
		sprintf(arg6,"all");
    i=_spawnl(_P_WAIT,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6,NULL);

    if (i == 0) {
        return LOWLEVEL_NO_ERROR;
    } else {
        sprintf(Message, "%s %s %s %s %s %s %s returned non-zero returncode %d",
                netshPath, arg1, arg2, arg3, arg4, arg5, arg6, i);
        return LOWLEVEL_ERROR_UNSPEC;
    }
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
    sprintf(arg7,"preferredlifetime=%u", prefered);
    sprintf(arg8,"validlifetime=%u", valid);

    sprintf(buf, "%s %s %s %s %s %s %s %s %s %s", arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
    i=_spawnl(_P_WAIT,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10, NULL);

    // if error code is non-zero, then the addition failed. One of the reasons why this
    // could happen is because the address or prefix already exists, so we'll try
    // to update its parameters (if this is RENEW/REBIND)
    if (i) {
        i = _spawnl(_P_WAIT, netshPath, netshPath, arg1, arg2, "set", arg4, arg5, arg6, arg7, arg8, arg9, arg10, NULL);
    }
    return i;
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
    i=_spawnl(_P_WAIT,netshPath,netshPath,arg1,arg2,arg3,arg4,arg5,arg6, NULL);

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

int execute(const char *filename, const char * argv[], const char *env[])
{
    intptr_t i;
    i=_spawnvpe(_P_WAIT, filename, argv, env);
    return i;
}

int get_mac_from_ipv6(const char* iface_name, int ifindex, const char* v6addr,
                      char* mac, int* mac_len) {
    /// @todo: Implement MAC reading for Windows
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

/** @brief returns host name of this host
 *
 * @param hostname buffer (hostname will be stored here)
 * @param hostname_len length of the buffer
 * @return LOWLEVEL_NO_ERROR if successful, appropriate LOWLEVEL_ERROR_* otherwise
 */
int get_hostname(char* hostname, int hostname_len) {
    memset(hostname,0, hostname_len);
    if (GetComputerNameExA(ComputerNameDnsFullyQualified, hostname, &hostname_len)) {
        return LOWLEVEL_NO_ERROR;
    }
    return LOWLEVEL_ERROR_UNSPEC;
}

void fill_random(uint8_t* buffer, int len) {
    unsigned int number;
    int i = 0;

    for (i=0; i < len; ++i) {
        rand_s(&number);
        buffer[i] = (number%256);
    }
}

extern int sock_add_tcp(char * ifacename,int ifaceid, char * addr, int port) {


    int on = 1,sResult=0;

    int error;
    WSADATA wsaData;
    SOCKET Insock = INVALID_SOCKET;


    // Initialize Winsock
    sResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (sResult != 0) {
        sprintf(Message, "WSAStartup faild. Something is wrong?");
        return LOWLEVEL_ERROR_SOCK_OPTS;
    }

    struct addrinfo *res, *rp;
    struct addrinfo hints;
    fd_set master_set;

    char port_char[6];
    char * tmp;
    int connectionNumber =1;
    ifaceid = 6;

//#ifdef LOWLEVEL_DEBUG
    printf("\n ### iface: %s(id=%d), addr=%s, port=%d \n", ifacename,ifaceid, addr, port);
    fflush(stdout);
//#endif

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    if( (error = getaddrinfo(NULL,  port_char, &hints, &res)) ) {
        sprintf(Message, "getaddrinfo failed. Is IPv6 protocol supported by kernel?");
        return LOWLEVEL_ERROR_GETADDRINFO;
    }

    if (port > 0) {

        sprintf(port_char,"%d",port);
        if( (listenSocket = socket(AF_INET6, SOCK_STREAM,0 )) == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            freeaddrinfo();
            WSACleanup();
            return LOWLEVEL_ERROR_UNSPEC;
        } else {
            printf("\n TCP socket has been created correctly\n " );
        }

        sResult=setsockopt(Insock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on) );
        if ( sResult== SOCKET_ERROR) {
            printf("SetSockOpt function failed: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return LOWLEVEL_ERROR_SOCK_OPTS;
        }

        /* Set the options  to receivce ipv6 traffic */
        sResult = setsockopt(Insock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on));
        if (sResult == SOCKET_ERROR) {
          printf("Unable to set up socket option IPV6_RECVPKTINFO with error %d\n",WSAGetLastError());
          freeaddrinfo(result);
          WSACleanup();
          return LOWLEVEL_ERROR_SOCK_OPTS;
        }

        // set socket as nonblocking
        sResult = ioctlsocket(Insock, FIONBIO, (char *)&on);
        if (sResult == SOCKET_ERROR) {
           printf("Unable to set up socket as nonblocking - ioctlsocket's failure with error %d\n",WSAGetLastError());
           freeaddrinfo(result);
           WSACleanup();
           return LOWLEVEL_ERROR_SOCK_OPTS;
        }

        freeaddrinfo(res);

        //initialize master fd_set
        FD_ZERO(&master_set);
        FD_SET(Insock,&master_set);
    }
    /* Open a TCP socket for inbound traffic */

   // struct sockaddr_in6 bindmeClient;

    //TCP client part
    if (port == 0) {

        port=547;
        sprintf(port_char,"%d",port);

        /* bind socket to a specified port */
        /*bzero(&bindmeClient, sizeof(struct sockaddr_in6));
        bindmeClient.sin6_family = AF_INET6;
        bindmeClient.sin6_port   = htons(port);
        bindmeClient.sin6_family = IPPROTO_TCP;
        tmp = (char*)(&bindmeClient.sin6_addr);
        inet_pton6(addr, tmp);*/
        //bindmeClient.sin6_addr = addr;

        memset(&hints,0,sizeof(hints));
        memset(&res,0,sizeof(res));

        hints.ai_flags    = AI_NUMERICSERV;
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

       /* if ((error = getaddrinfo(hostp->hostname, hostp->port,
                    &hints, &res)) != 0) */

        printf("\n ### iface: %s(id=%d), addr=%s, port=%d \n", ifacename,ifaceid, addr, port);

        if( (error = getaddrinfo(addr, port_char, &hints, &res)) ) {
            sprintf(Message, "getaddrinfo failed. Is IPv6 protocol supported by kernel?");
            return LOWLEVEL_ERROR_GETADDRINFO;
        } else {
            printf ("\n getaddrinfo called correctly\n");
            //printf("\n addr: %s\n", res->ai_addr);
        }
        //(connect(Insock, (struct sockaddr_in6*)&bindmeClient, sizeof(struct sockaddr_in6)) != 0)

        for(rp=res; rp!=NULL;rp->ai_next) {
            Insock = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
            if(Insock==-1) {
                continue;
            } else {
                printf("\n TCP socket has been created correctly\n " );
            }
            if (connect(Insock, rp->ai_addr, rp->ai_addrlen) != 0) {
                 Rerror("Unable to connect with DHCP server, connect function failed");
            } else {
                 result++;
                 break;
            }
        }

        if (result==0) {
            printf("\n Can't connect on any interface with specified host");
            return LOWLEVEL_ERROR_CONNECT_FAILED;
        } else {
            printf("\n Connected with host");
            return Insock;
        }
    }
    result=0;
    //TCP server part
    struct sockaddr_in6 bindmeServer;
    if ( port > 0) {

        /* bind socket to a specified port */
        bzero(&bindmeServer, sizeof(struct sockaddr_in6));
        bindmeServer.sin6_family = AF_INET6;
        bindmeServer.sin6_port   = htons(port);
        tmp = (char*)(&bindmeServer.sin6_addr);
        inet_pton6(addr, tmp);

        socklen_t bindmeServerSize;
        bindmeServerSize= sizeof (struct sockaddr_in6);

        printf("\n Socket FD: %d", Insock );
       // printf("\n Size of struct passed to bind:%d\n",sizeof(struct sockaddr_in6));
        //printf("\n Size of struct passed to bind:%d",sizeof(struct sockaddr_in6));
        result = bind( Insock,(struct sockaddr_in6 *)&bindmeServer,bindmeServerSize);
        printf("\n Result:%d \n",result);

        if (result == SOCKET_ERROR) {
            printf("Unable to bind socket: %d\n",WSAGetLastError());
            freeaddrinfo(result);
            closesocket(Insock);
            WSACleanup();
            return LOWLEVEL_ERROR_BIND_FAILED;
        } else {
            sprintf(Message, "Socket has been bind succesfully");
            printf("\n Socket has been bind succesfully \n");
        }

        if (connectionNumber > 0)  {
             sResult=listen (Insock,connectionNumber);
             if ( sResult == SOCKET_ERROR) {
                 printf("\n Listen function has NOT been called correctly with error:%d\n", WSAGetLastError());
                 closesocket(Insock);
                 WSACleanup();
                 return LOWLEVEL_ERROR_LISTEN_FAILED;
             } else {
                 sprintf(Message,"Listen function has been called correctly");
                 printf("\n Listen function has been called correctly \n");
             }
        } else {
            sprintf(Message, "Connection number hasn't been specified");
            return LOWLEVEL_ERROR_LISTEN_FAILED;
        }

    }

    printf("\nRETURN SOCK FD:%d\n",Insock);
    return Insock;


}



extern int getsOpt(int fd) {

    int len, sockType, result;

    len = sizeof(sockType);

    if (result = getsockopt(fd,SOL_SOCKET,SO_TYPE,&sockType,&len) ) {
        Rerror("Getsockopt function failed");
        return -1;
    } else {
        sprintf(Message, "Getsockopt OK");
        return result;
    }
    return -1;
}



extern int accept_tcp (int fd,char * peerPlainAddr) {

    struct sockaddr_in6 peerStructure;

    bzero(&peerStructure, sizeof(struct sockaddr_in6));
    size_t peerStructureLen = sizeof(peerStructure);

    fd_new = accept(fd,(struct sockaddr_in6 *)&peerStructure,peerStructureLen);
    if (fd_new < 0) {
        //sprintf(Message, "Accept function failed. Cannot create net socket descriptor");
        //close(fd_new);

        if (errno !=  WSAEWOULDBLOCK) {
            printf("Cannot create net socket descriptor.Accept function failed with error %d\n",WSAGetLastError());
            WSACleanup();
            //Here should be check if client didn't close connection
            //close(fd_new);
            return -1;
        }

    }
    /* get source address */
    inet_ntop6((void*)&peerStructure.sin6_addr, peerPlainAddr);
    return fd_new;

}

extern int getPeerName_ipv6(int fd,struct socketStruct,char * addr) {

     addrLength = sizeof(socketStruct.sockaddr_in);
     if(getpeername(fd, addr, addrLength) < 0) {
         printf( "Getpeername function failed with error: %ld\nCannot return peername address\n", WSAGetLastError() );
         closesocket(fd_new);
         WSACleanup();
         return 1;
     } else {
         return 0;
     }

}

extern int sock_recv_tcp(int fd, char * recvBuffer, char *myPlainAddr, char *peerPlainAddr, int bufLength, int flags) {

    int iResult;
    iResult = recv (fd, recvBuffer, bufLength, flags);
    if (iResult < 0) {
        printf ("Cannot receive data, receive function socket error %d\n",WSAGetLastError());
        WSACleanup();
        return LOWLEVEL_ERROR_UNSPEC;
    } else {
        /* get source address */
        inet_ntop6((void*)&peerAddr.sin6_addr, peerPlainAddr);

        /* get destination address */
        for(cm = (struct cmsghdr *) CMSG_FIRSTHDR(&msg); cm; cm = (struct cmsghdr *) CMSG_NXTHDR(&msg, cm)){
        if (cm->cmsg_level != IPPROTO_IPV6 || cm->cmsg_type != IPV6_PKTINFO)
            continue;
        pktinfo= (struct in6_pktinfo *) (CMSG_DATA(cm));
        inet_ntop6((void*)&pktinfo->ipi6_addr, myPlainAddr);
        }
        return iResult;
    }
}


extern int sock_send_tcp(int fd,char *buf, int buflen, int flags, int port ) {

    struct addrinfo hints, *res;
    int sResult = 0;
    char cport[10];

    sprintf(cport,"%d",port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_STREAM;


    if (getaddrinfo(addr, cport, &hints, &res) < 0) {
        return -1; /* Error in transmitting */
    }

    if (!buflen){
        buflen=(int)strlen(buf);
    }
    sResult = send (fd,buf,buflen,flags);

    if (sResult == SOCKET_ERROR)
    {
        printf(Message, "Unable to send data (dst addr: %s) with error %d\n", addr,WSAGetLastError());
        return LOWLEVEL_ERROR_SOCKET;
    } else {
        printf("\n %d bytes has been send\n",iResult);
    }

    freeaddrinfo(res);

    return sResult;
}

extern int terminate_tcp_connection(int fd,int how) {

    //SD_RECEIVE 0
    //SD_SEND    1
    //SD_BOTH    2

    iResult = shutdown(fd,how);
    if (iResult == SOCKET_ERROR) {
        wprintf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    return iResult;

}

