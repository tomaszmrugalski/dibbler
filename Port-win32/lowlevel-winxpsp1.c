/*
 * $Id: lowlevel-winxpsp1.c,v 1.7 2004-09-28 16:01:49 thomson Exp $
 *
 *  $Log: not supported by cvs2svn $
 *  Revision 1.6  2004/05/24 21:16:37  thomson
 *  Various fixes.
 *
 *  Revision 1.4  2004/03/28 19:48:10  thomson
 *  Problem with missing IPv6 stack solved.
 *
 * Released under GNU GPL v2 licence                                
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

#include "portable.h"

char ipv6Path[256];
char cmdPath[256];

// Find ipv6.exe
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
	memcpy(ipv6Path, buf,256);
	strcpy(buf+i,"\\system32\\cmd.exe");
	memcpy(cmdPath, buf,256);
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
  
extern	struct iface* if_list_get()
{
	char* buffer=NULL;
	unsigned long buflen;
	struct  iface* iface;
	struct  iface* retval;
	struct  sockaddr_in6 *addrpck;
    char*  addr;    
    int linkLocalAddrCnt;
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
	while(adaptaddr)	
	{
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
    
        linkaddr=adaptaddr->FirstUnicastAddress;
        //for evert unicast address on iface
        linkLocalAddrCnt=0;
        while(linkaddr)
        {
              addrpck=(struct sockaddr_in6*)linkaddr->Address.lpSockaddr;
              if (IN6_IS_ADDR_LINKLOCAL(( (struct in6_addr*) (&addrpck->sin6_addr)))&&
                  (linkaddr->PreferredLifetime>0))
                linkLocalAddrCnt++;
            linkaddr=linkaddr->Next;
        }
        
        iface->linkaddrcount=linkLocalAddrCnt;
        if(linkLocalAddrCnt>0)
        {
            iface->linkaddr=addr=(char*)malloc(linkLocalAddrCnt*16);
            
            linkaddr=adaptaddr->FirstUnicastAddress;
            while(linkaddr)
            {
                  addrpck=(struct sockaddr_in6*)linkaddr->Address.lpSockaddr;
                  if (IN6_IS_ADDR_LINKLOCAL(( (struct in6_addr*) (&addrpck->sin6_addr)))&&
                  (linkaddr->PreferredLifetime>0))  
                {
                    memcpy(addr,&(addrpck->sin6_addr),16);
                    addr+=16;
                }
                linkaddr=linkaddr->Next;
            }
        }
        else
            iface->linkaddr=NULL;

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
    
    if (found)
        retVal=found->DadState==IpDadStateDuplicate;
    free(buffer);
        return retVal;
}
extern int ipaddr_add(char * ifacename, int ifaceid, char * addr, long pref, long valid)
{
	char NrStr[12];
	char arg1[]="/C";
	char arg3[]="adu";
	char *arg4=(char *)malloc(strlen(addr)+13);
	char arg5[]="life";
	char arg6[25];
	char arg7[]=">>NULL";
	intptr_t i;
	itoa(ifaceid,NrStr,10);
	strcpy(arg4,NrStr);
	strcat(arg4,"/");
	strcat(arg4,addr);

	ltoa(valid,NrStr,10);
	strcpy(arg6,NrStr);
	strcat(arg6,"/");
	ltoa(pref,NrStr,10);
	strcat(arg6,NrStr);
//	FreeConsole();
	i=_spawnl(_P_WAIT,ipv6Path,ipv6Path,arg3,arg4,arg5,arg6,NULL);

	return i;
}

extern int ipaddr_del(char * ifacename, int ifaceid, char * addr)
{
	return ipaddr_add(NULL,ifaceid, addr,0, 0);
}

SOCKET mcast=0;

extern int sock_add(char * ifacename,int ifaceid, char * addr, int port, int thisifaceonly)
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

//	if (!IN6_IS_ADDR_MULTICAST((IN6_ADDR*)addrpack))
//		return s;
//	}

	memset(&bindme, 0, sizeof(bindme));
	bindme.sin6_family   = AF_INET6;
	bindme.sin6_port     = htons(port);
	if (!IN6_IS_ADDR_MULTICAST((IN6_ADDR*)packedAddr)) 	{
		// unicast 
        inet_pton6(addr, (char*)&bindme.sin6_addr);
	} else {
		// multicast
		//bindme.sin6_scope_id = ifaceid;
	}

	if (!mcast) {
	  mcast = s;
      if (bind(s, (struct sockaddr*)&bindme, sizeof(bindme))) {
	      displayError(WSAGetLastError());		
		  return -4;
	  }
	} else {
      s = mcast;
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
	ADDRINFO			inforemote,*remote;
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
	if(!(readBytes=recvfrom(fd,buf,buflen,0,(SOCKADDR*)&info,&infolen)))
		return -1;
	else
	{
		inet_ntop6(info.sin6_addr.u.Byte,peerPlainAddr);
		return	readBytes;
	}
} 
