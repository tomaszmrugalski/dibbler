
/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: lowlevel-macos.c,v 1.1 2008-07-20 15:54:37 thomson Exp $
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "Portable.h"

/*
#define LOWLEVEL_DEBUG 1
*/

char Message[1024] = {0};

int lowlevelInit()
{
    return LOWLEVEL_NO_ERROR;
}

int lowlevelExit()
{
  return LOWLEVEL_NO_ERROR;
}

void if_list_release(struct iface * list) {
    struct iface * tmp;
    while (list) {
        tmp = list->next;
	if (list->linkaddrcount)
	    free(list->linkaddr);
	if (list->globaladdrcount)
	    free(list->globaladdr);
        free(list);
        list = tmp;
    }
}

/*
 * returns interface list with detailed informations
 */
struct iface * if_list_get()
{
    /* implement this: get interface list somehow and then create list of iface structures */
    /* TODO: implement this */
    return 0;
}

int ipaddr_add(const char * ifacename, int ifaceid, const char * addr, unsigned long pref,
	       unsigned long valid, int prefixLength)
{
    /* TODO: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

int ipaddr_update(const char* ifacename, int ifindex, const char* addr,
		  unsigned long pref, unsigned long valid, int prefixLength)
{
    /* TODO: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}


int ipaddr_del(const char * ifacename, int ifaceid, const char * addr, int prefixLength)
{
    /* TODO: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

int sock_add(char * ifacename,int ifaceid, char * addr, int port, int thisifaceonly, int reuse)
{
    int error;
    int on = 1;
    struct addrinfo hints;
    struct addrinfo *res, *res2;
    struct ipv6_mreq mreq6; 
    int Insock;
    int multicast;
    char port_char[6];
    char * tmp;
    struct sockaddr_in6 bindme;
    sprintf(port_char,"%d",port);

    if (!strncasecmp(addr,"ff",2))
	multicast = 1;
    else
	multicast = 0;

#ifdef LOWLEVEL_DEBUG
    printf("### iface: %s(id=%d), addr=%s, port=%d, ifaceonly=%d reuse=%d###\n",
    ifacename,ifaceid, addr, port, thisifaceonly, reuse);
    fflush(stdout);
#endif
    
    /* Open a socket for inbound traffic */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    if( (error = getaddrinfo(NULL,  port_char, &hints, &res)) ){
	sprintf(Message, "getaddrinfo failed. Is IPv6 protocol supported by kernel?");
	return LOWLEVEL_ERROR_GETADDRINFO;
    }
    if( (Insock = socket(AF_INET6, SOCK_DGRAM,0 )) < 0){
	sprintf(Message, "socket creation failed. Is IPv6 protocol supported by kernel?");
	return LOWLEVEL_ERROR_UNSPEC;
    }	

#if 0
    /* this part looks like Linux only code */
    /* Set the options  to receivce ipv6 traffic */
    if (setsockopt(Insock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on)) < 0) {
	sprintf(Message, "Unable to set up socket option IPV6_RECVPKTINFO.");
	return LOWLEVEL_ERROR_SOCK_OPTS;
    }
#endif

    if (thisifaceonly) {
#if 0
      /* this part also looks like linux only code */
	if (setsockopt(Insock, SOL_SOCKET, SO_BINDTODEVICE, ifacename, strlen(ifacename)+1) <0) {
	    sprintf(Message, "Unable to bind socket to interface %s.", ifacename);
	    return LOWLEVEL_ERROR_BIND_IFACE;
	}
#endif
    }

    /* allow address reuse (this option sucks - why allow running multiple servers?) */
    if (reuse!=0) {
	if (setsockopt(Insock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)  {
	    sprintf(Message, "Unable to set up socket option SO_REUSEADDR.");
	    return LOWLEVEL_ERROR_REUSE_FAILED;
	}
    }

    /* bind socket to a specified port */
    bzero(&bindme, sizeof(struct sockaddr_in6));
    bindme.sin6_family = AF_INET6;
    bindme.sin6_port   = htons(port);
    tmp = (char*)(&bindme.sin6_addr);
    inet_pton6(addr, tmp);
    if (bind(Insock, (struct sockaddr*)&bindme, sizeof(bindme)) < 0) {
	sprintf(Message, "Unable to bind socket: %s", strerror(errno) );
	return LOWLEVEL_ERROR_BIND_FAILED;
    }

    freeaddrinfo(res);

    /* multicast server stuff */
    if (multicast) {
	hints.ai_flags = 0;
	if((error = getaddrinfo(addr, port_char, &hints, &res2))){
	    sprintf(Message, "Failed to obtain getaddrinfo");
	    return LOWLEVEL_ERROR_GETADDRINFO;
	}
	memset(&mreq6, 0, sizeof(mreq6));
	mreq6.ipv6mr_interface = ifaceid;
	memcpy(&mreq6.ipv6mr_multiaddr, &((struct sockaddr_in6 *)res2->ai_addr)->sin6_addr,
	       sizeof(mreq6.ipv6mr_multiaddr));
	
	/* Add to the all agent multicast address */
#if 0
	/* there seem to be on IPV6_ADD_MEMBERSHIP, only IP_ADD_MEMBERSHIP. Will it work? */
	if (setsockopt(Insock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq6, sizeof(mreq6))) {
	    sprintf(Message, "error joining ipv6 group");
	    return LOWLEVEL_ERROR_MCAST_MEMBERSHIP;
	}
#endif
	freeaddrinfo(res2);
    }
    
    return Insock;
}

int sock_del(int fd)
{
    return close(fd);
}

int sock_send(int sock, char *addr, char *buf, int message_len, int port, int iface )
{
    struct addrinfo hints, *res;
    int result;
    char cport[10];
    sprintf(cport,"%d",port);
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    if (getaddrinfo(addr, cport, &hints, &res) < 0) {
	return -1; /* Error in transmitting */
    }

    result = sendto(sock, buf, message_len, 0, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    if (result<0) {
	sprintf(Message, "Unable to send data (dst addr: %s)", addr);
	return LOWLEVEL_ERROR_SOCKET;
    }
    return LOWLEVEL_NO_ERROR;
}

/*
 *
 */
int sock_recv(int fd, char * myPlainAddr, char * peerPlainAddr, char * buf, int buflen)
{
    struct msghdr msg;            /* message received by recvmsg */
    struct sockaddr_in6 peerAddr; /* sender address */
    struct sockaddr_in6 myAddr;   /* my address */
    struct iovec iov;             /* simple structure containing buffer address and length */

    struct cmsghdr *cm;           /* control message */
    struct in6_pktinfo *pktinfo; 

    char control[CMSG_SPACE(sizeof(struct in6_pktinfo))];
    char controlLen = CMSG_SPACE(sizeof(struct in6_pktinfo));
    int result = 0;
    bzero(&msg, sizeof(msg));
    bzero(&peerAddr, sizeof(peerAddr));
    bzero(&myAddr, sizeof(myAddr));
    bzero(&control, sizeof(control));
    iov.iov_base = buf;
    iov.iov_len  = buflen;

    msg.msg_name       = &peerAddr;
    msg.msg_namelen    = sizeof(peerAddr);
    msg.msg_iov        = &iov;
    msg.msg_iovlen     = 1;
    msg.msg_control    = control;
    msg.msg_controllen = controlLen;

    result = recvmsg(fd, &msg, 0);

    if (result==-1) {
	return LOWLEVEL_ERROR_UNSPEC;
    }

    /* get source address */
    inet_ntop6((void*)&peerAddr.sin6_addr, peerPlainAddr);

    /* get destination address */
    for(cm = (struct cmsghdr *) CMSG_FIRSTHDR(&msg); cm; cm = (struct cmsghdr *) CMSG_NXTHDR(&msg, cm)){
	if (cm->cmsg_level != IPPROTO_IPV6 || cm->cmsg_type != IPV6_PKTINFO)
	    continue;
	pktinfo= (struct in6_pktinfo *) (CMSG_DATA(cm));
	inet_ntop6((void*)&pktinfo->ipi6_addr, myPlainAddr);
    }
    return result;
}

void microsleep(int microsecs)
{
    struct timespec x,y;

    x.tv_sec  = (int) microsecs / 1000000;
    x.tv_nsec = ( microsecs - x.tv_sec*1000000) * 1000;
    nanosleep(&x,&y);
}

/*
 * returns: -1 - address not found, 0 - addr is ok, 1 - addr is tentative
 */
int is_addr_tentative(char * ifacename, int iface, char * addr)
{
    /* TODO: implement this */
    return 0;
}

uint32_t getAAASPIfromFile() {
    char filename[1024];
    struct stat st;
    uint32_t ret;
    FILE *file;

    strcpy(filename, "/var/lib/dibbler/AAA/AAA-SPI");

    if (stat(filename, &st))
        return 0;

    file = fopen(filename, "r");
    if (!file)
        return 0;

    fscanf(file, "%x", &ret);
    fclose(file);

    return ret;
}

char * getAAAKeyFilename(uint32_t SPI)
{
    static char filename[1024];
    if (SPI != 0)
        snprintf(filename, 1024, "%s%s%x", "/var/lib/dibbler/AAA/", "AAA-key-", SPI);
    else
        strcpy(filename, "/var/lib/dibbler/AAA/AAA-key");
    return filename;
}

char * getAAAKey(uint32_t SPI, unsigned *len) {
    char * filename = 0;
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

    /* FIXME should be freed somewhere */
    retval = malloc(st.st_size);
    if (!retval) {
        close(fd);
        return NULL;
    }

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

