/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *          win2k version by <sob@hisoftware.cz>
 *
 * based on code from Dibbler 0.2.0-RC2 and Dibbler 0.4.0
 *
 * $Id: lowlevel-winnt2k.c,v 1.1 2005-07-23 14:33:22 thomson Exp $
 *
 * released under GNU GPL v2 licence
 *
 */

/* this file contains lowlevel functions for M$ Windows 2000. It uses ipv6.exe to perform
   various low level tasks. */

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <Ws2tcpip.h.>
#include <Ws2spi.h>

#include <tpipv6.h>

#include <iphlpapi.h>
#include <iptypes.h>
#include <process.h>
#include <errno.h>
#include <stdlib.h>

#include "Portable.h"

char ipv6Path[256];
char cmdPath[256];
char tmpFile[256];

/* 
  Find ipv6.exe
 */
int lowlevelInit()
{
    char buf[256];
    FILE *f;
    int i;
    i = GetEnvironmentVariable("SYSTEMROOT",buf, 256);
    if (!i) {
	printf("Environment variable SYSTEMROOT not set.\n");
	return 0;
    }
    strcpy(buf+i,"\\system32\\ipv6.exe");
    if (!(f=fopen(buf,"r"))) {
	printf("Unable to open %s file.\n",buf);
	return 0;
    }
    fclose(f);
    memcpy(ipv6Path, buf, 256);
    strcpy(buf+i,"\\system32\\cmd.exe");
    memcpy(cmdPath, buf, 256);
    //
    strcpy(buf+i,"\\dibbler-ipv6.tmp");
    memcpy(tmpFile, buf, 256);
    //
    return 1;
}

char * displayError(int errCode) {
	static char Message[1024];
	printf("Error %d:",errCode);
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                  FORMAT_MESSAGE_MAX_WIDTH_MASK,
                  NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPSTR)Message, 1024, NULL);
	printf("%s\n",Message);
	return Message;
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

extern struct iface* if_list_get()
{
    struct iface * head = NULL;
    struct iface * tmp;
    int pos;
    char * bufPtr;
    char buf[512];
    char * name;
    char * tmpbuf;
    FILE *f;

    int result = spawnl(_P_WAIT,cmdPath,cmdPath,"/C",ipv6Path,"if",">",tmpFile,NULL);
    if (result<0) {
        printf("ERROR: unable to run ipv6.exe\n");
        return NULL;
    }
    if ( ! (f=fopen(tmpFile,"r"))  ) {
        printf("ERROR: unable to open %s file\n",tmpFile);
        return NULL;
    }

    while ( !feof(f)) {
        fgets(buf,511,f);

        // Interface 1 (site 1): my network interface
        if (strstr(buf,"Interface ")) {
                tmp = (struct iface*)malloc(sizeof(struct iface));
                tmp->next = head;

                memset(tmp->mac,0,255);
                tmp->maclen = 0;
                tmp->linkaddrcount = 0;
                tmp->hardwareType = 1; // other (ipifcons.h)
                tmp->flags = 0;
                head = tmp;
                printf(">>>%s",(buf+10));
                tmp->id = atoi(buf+10);
                name = (strstr(buf+10,":")+2);
                name[ strlen(name)-1] = 0;
                sprintf(tmp->name,name);
        }
        // link-level address: 00-11-22-33-44-55
        if (strstr(buf,"link-level address:")) {
                printf("###(%d)%s",strlen(buf+22),buf+22);
                if (strlen(buf+22)==18) { // ethernet
                    tmp->hardwareType = 6;
                    tmp->flags = IF_UP | IF_RUNNING | IF_MULTICAST;
                    sscanf( (buf+22) ,"%x-%x-%x-%x-%x-%x", tmp->mac, tmp->mac+1, tmp->mac+2,
                                                            tmp->mac+3, tmp->mac+4, tmp->mac+5);
                    tmp->maclen=6;
                } else
                if (strlen(buf+22) > 7) { // tunnel 0.0.0.0
                    tmp->hardwareType = 131; // tunnel
                    sscanf( (buf+22), "%d.%d.%d.%d", tmp->mac, tmp->mac+1,
                                                     tmp->mac+2, tmp->mac+3);
                    tmp->maclen = 4;
                } else
                if (strlen(buf+22) < 2) { // loopback ""
                    tmp->hardwareType = 24; // loopback
                    tmp->flags = IF_UP | IF_RUNNING | IF_LOOPBACK | IF_MULTICAST;
                    tmp->maclen = 0;
                } else {
                    tmp->hardwareType = 1; // unknown
                    tmp->flags = IF_UP | IF_RUNNING | IF_MULTICAST;
                    // uncomment this if you like cute defaults
                    // tmp->hardwareType = 132; // coffee pot
                }
        }
        // link-local address
        if (strstr(buf,"fe80::")) {
                bufPtr = strstr(buf,",");
                if (bufPtr) *bufPtr = 0;
                printf("ADDR=[%s]",strstr(buf,"fe80::")); fflush(stdout);
                pos = (tmp->linkaddrcount + 1)*16;
                printf("Alokuje %d pamieci.\n",pos);
                tmpbuf = (char*) malloc( pos );
                memcpy(tmpbuf,tmp->linkaddr, pos - 16);
                inet_pton6( strstr(buf,"fe80::"), tmpbuf + tmp->linkaddrcount*16);
                free(tmp->linkaddr);
                tmp->linkaddr = tmpbuf;
                tmp->linkaddrcount++;
        }
    }
    fclose(f);
	return tmp;
}

extern int is_addr_tentative(char* ifacename, int iface, char* plainAddr)
{
    int result;
    char buf[512];
    FILE *f;

    sprintf(buf, "%d",iface);
    result = spawnl(_P_WAIT,cmdPath,cmdPath,"/C",ipv6Path,"if",buf,">",tmpFile,NULL);
    if (result<0) {
        printf("ERROR: unable to run ipv6.exe\n");
        return -1;
    }
    if ( ! (f=fopen(tmpFile,"r"))  ) {
        printf("ERROR: unable to open %s file\n",tmpFile);
        return -1;
    }

    while ( !feof(f)) {
        fgets(buf,511,f);
        if (strstr(buf,plainAddr) && strstr(buf,"duplicate") ) {
                fclose(f);
                return 1; // duplicate
        }
    }
    fclose(f);
    return 0; // address ok
}

extern int ipaddr_add(const char* ifacename, int ifindex, const char* addr,
		      unsigned long pref, unsigned long valid)
{
    char addr2[128];
    char buf[64];
    int result;

    sprintf(buf,"%d/%d",valid,pref);
    sprintf(addr2,"%d/%s", ifindex, addr);
    result = spawnl(_P_WAIT,cmdPath,cmdPath,"/C",ipv6Path,
                        "adu",addr2,"lifetime",buf,">",tmpFile,NULL);
    if (result<0) {
        return result;
    }
	return 0;
}

extern int ipaddr_del(const char* ifacename, int ifindex, const char* addr)
{
	return ipaddr_add(ifacename, ifindex, addr, 0, 0);
}

SOCKET mcast=0;

extern int sock_add(char * ifacename,int ifaceid, char * addr, int port, int thisifaceonly, int reuse)
{
    SOCKET s;
    struct sockaddr_in6 bindme;
    struct ipv6_mreq ipmreq; 
    int	hops=1;
    char packedAddr[16];
    //char multiAddr[16] = { 0xff,2, 0,0, 0,0, 0,0, 0,0, 0,0, 0,1, 0,2};
    
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
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&hops, sizeof(hops)))
	return -9;

    if (bind(s, (struct sockaddr*)&bindme, sizeof(bindme))) {
        // displayError(WSAGetLastError());		
        return -4;
    }
    
    if (IN6_IS_ADDR_MULTICAST((IN6_ADDR*)packedAddr)) {
	/* multicast */
	ipmreq.ipv6mr_interface=ifaceid;
	memcpy(&ipmreq.ipv6mr_multiaddr,packedAddr,16);
	if(setsockopt(s,IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,(char*)&ipmreq,sizeof(ipmreq)))
	    return -6;
	if(setsockopt(s,IPPROTO_IPV6,IPV6_MULTICAST_HOPS,(char*)&hops,sizeof(hops)))
	    return -5;
    } 
    return s;
}

int sock_del(int fd)
{
	return closesocket(fd);
}
int sock_send(int fd, char * addr, char * buf, int buflen, int port,int iface)
{	
    struct addrinfo inforemote,*remote;
	char				addrStr[sizeof("ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")+5];
	char				portStr[10];
	int i;
	char				packaddr[16];
	char				ifaceStr[10];

	strcpy(addrStr,addr);
	itoa(port,portStr,10);
    itoa(iface,ifaceStr,10);
	inet_pton6(addrStr,packaddr);

    if(IN6_IS_ADDR_LINKLOCAL((struct in6_addr*)packaddr)
       ||IN6_IS_ADDR_SITELOCAL((struct in6_addr*)packaddr))
	strcat(strcat(addrStr,"%"),ifaceStr);

#if 0
/*	if (IN6_IS_ADDR_MULTICAST((IN6_ADDR*)addr))
	{
		ipmreq.ipv6mr_interface=4;
		memcpy(&ipmreq.ipv6mr_multiaddr,addr,16);
		if(setsockopt(fd,IPPROTO_IPV6,IPV6_ADD_MEMBERSHIP,(char*)&ipmreq,sizeof(ipmreq)))
			return -1; //WSAGetLastError();
		//int hops=8;
		//if(setsockopt(fd,IPPROTO_IPV6,IPV6_MULTICAST_HOPS,(char*)&hops,sizeof(hops)))
		//	return -1; //WSAGetLastError();
	}*/
#endif
	
	memset(&inforemote, 0, sizeof(inforemote));
	inforemote.ai_flags=AI_NUMERICHOST;
	inforemote.ai_family=PF_INET6;
	inforemote.ai_socktype=SOCK_DGRAM;
	inforemote.ai_protocol=IPPROTO_IPV6;
	//inet_ntop6(addr,addrStr);
	if(getaddrinfo(addrStr,portStr,&inforemote,&remote))
		return 0;
	if (i=sendto(fd,buf,buflen,0,remote->ai_addr,remote->ai_addrlen))
	{
		freeaddrinfo(remote);
    	if (i<0) displayError(WSAGetLastError());
		return 0;
	}
/*	if((setsockopt(fd,IPPROTO_IPV6,IPV6_DROP_MEMBERSHIP,(char*)&ipmreq,sizeof(ipmreq))))
		return WSAGetLastError();*/
	freeaddrinfo(remote);
	return i;
}

int sock_recv(int fd, char * myPlainAddr, char * peerPlainAddr, char * buf, int buflen)
{
    struct sockaddr_in6 info;  
    int	infolen ;		
    int	readBytes;
	
	infolen=sizeof(info);
	if(!(readBytes=recvfrom(fd,buf,buflen,0,(SOCKADDR*)&info,&infolen))) {
		return -1;
	} else {
#ifdef MINGWBUILD
    	        inet_ntop6(info.sin6_addr._S6_un._S6_u8,peerPlainAddr);    	        
#else
    	        inet_ntop6(info.sin6_addr.u.Byte,peerPlainAddr);
#endif    	        
		return	readBytes;
	}
} 

extern int dns_add(const char* ifname, int ifindex, const char* addrPlain) {
  // I think Windows NT/2000 does not support DNS over IPv6...
    return 0;
}
extern int dns_del(const char* ifname, int ifindex, const char* addrPlain) {
  // I think Windows NT/2000 does not support DNS over IPv6...
    return 0;
}

extern int domain_add(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int domain_del(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int ntp_add(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int ntp_del(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int timezone_set(const char* ifname, int ifindex, const char* timezone) {
    return 0;
}

extern int timezone_del(const char* ifname, int ifindex, const char* timezone) {
    return 0;
}

extern int sipserver_add(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int sipserver_del(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int sipdomain_add(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int sipdomain_del(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int nisserver_add(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int nisserver_del(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int nisdomain_set(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int nisdomain_del(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int nisplusserver_add(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int nisplusserver_del(const char* ifname, int ifindex, const char* addrPlain) {
    return 0;
}

extern int nisplusdomain_set(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

extern int nisplusdomain_del(const char* ifname, int ifindex, const char* domain) {
    return 0;
}

/*
 * $Log: not supported by cvs2svn $
 */
