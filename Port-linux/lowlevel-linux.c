/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 * 
 * This file is a based on the ip/ipaddress.c file from iproute2
 * ipaddress.c authors (note: those folks are not involved in Dibbler development):
 * Authors: Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
 * Changes: Laszlo Valko <valko@linux.karinthy.hu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: lowlevel-linux.c,v 1.13 2009-03-09 22:27:23 thomson Exp $
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fnmatch.h>
#include <time.h>
#include <errno.h>


#include "libnetlink.h"
#include "ll_map.h"
#include "utils.h"
#include "rt_names.h"
#include "Portable.h"

/*
#define LOWLEVEL_DEBUG 1
*/

#ifndef IPV6_RECVPKTINFO
#define IPV6_RECVPKTINFO IPV6_PKTINFO
#endif

#define ADDROPER_DEL 0
#define ADDROPER_ADD 1
#define ADDROPER_UPDATE 2

struct rtnl_handle rth;
char Message[1024] = {0};

int lowlevelInit()
{
    if (rtnl_open(&rth, 0) < 0) {
	sprintf(Message, "Cannot open rtnetlink\n");
	return LOWLEVEL_ERROR_SOCKET;
    }
    return 0;
}

int lowlevelExit()
{
    rtnl_close(&rth);
    return 0;
}

struct nlmsg_list
{
	struct nlmsg_list *next;
	struct nlmsghdr	  h;
};

int print_linkinfo(struct nlmsghdr *n);
int print_addrinfo(struct nlmsghdr *n);
int print_selected_addrinfo(int ifindex, struct nlmsg_list *ainfo);
void ipaddr_local_get(int *count, char **buf, int ifindex, struct nlmsg_list *ainfo);
void ipaddr_global_get(int *count, char **buf, int ifindex, struct nlmsg_list *ainfo);
void print_link_flags( unsigned flags);
int default_scope(inet_prefix *lcl);

int store_nlmsg(const struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
{
    struct nlmsg_list **linfo = (struct nlmsg_list**)arg;
    struct nlmsg_list *h;
    struct nlmsg_list **lp;
    
    h = malloc(n->nlmsg_len+sizeof(void*));
    if (h == NULL)
	return -1;
    
    memcpy(&h->h, n, n->nlmsg_len);
    h->next = NULL;
    
    for (lp = linfo; *lp; lp = &(*lp)->next) /* NOTHING */;
    *lp = h;
    
    ll_remember_index(who, n, NULL);
    return 0;
}

void release_nlmsg_list(struct nlmsg_list *n) {
    struct nlmsg_list *tmp;
    while (n) {
	tmp = n->next;
	free(n);
	n = tmp;
    }
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
    struct nlmsg_list *linfo = NULL;
    struct nlmsg_list *ainfo = NULL;
    struct nlmsg_list *l;
    struct rtnl_handle rth;
    struct iface * head = NULL;
    struct iface * tmp;
    int preferred_family = AF_PACKET;

    /* required to display information about interface */
    struct ifinfomsg *ifi;
    struct rtattr * tb[IFLA_MAX+1];
    int len;
    memset(tb, 0, sizeof(tb));
    memset(&rth,0, sizeof(rth));

    rtnl_open(&rth, 0);
    rtnl_wilddump_request(&rth, preferred_family, RTM_GETLINK);
    rtnl_dump_filter(&rth, store_nlmsg, &linfo, NULL, NULL);
    
    /* 2nd attribute: AF_UNSPEC, AF_INET, AF_INET6 */
    rtnl_wilddump_request(&rth, AF_UNSPEC, RTM_GETADDR);
    rtnl_dump_filter(&rth, store_nlmsg, &ainfo, NULL, NULL);

    /* build list with interface names */
    for (l=linfo; l; l = l->next) {
	ifi = NLMSG_DATA(&l->h);
	len = (&l->h)->nlmsg_len;
	len -= NLMSG_LENGTH(sizeof(*ifi));
	parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

#ifdef LOWLEVEL_DEBUG
	printf("### iface %d %s (flags:%d) ###\n",ifi->ifi_index,
	       (char*)RTA_DATA(tb[IFLA_IFNAME]),ifi->ifi_flags);
#endif

	tmp = malloc(sizeof(struct iface));
	memset(tmp, 0, sizeof(struct iface));
	snprintf(tmp->name,MAX_IFNAME_LENGTH,"%s",(char*)RTA_DATA(tb[IFLA_IFNAME]));
	tmp->id=ifi->ifi_index;
	tmp->flags=ifi->ifi_flags;
	tmp->hardwareType = ifi->ifi_type;
	tmp->next=head;
	head=tmp;
        /* printf("C: [%s,%d,%d]\n",tmp->name,tmp->id,tmp->flags); */

        memset(tmp->mac,0,255);
	/* This stuff reads MAC addr */
	/* Does inetface has LL_ADDR? */
	if (tb[IFLA_ADDRESS]) {
  	    tmp->maclen = RTA_PAYLOAD(tb[IFLA_ADDRESS]);
	    memcpy(tmp->mac,RTA_DATA(tb[IFLA_ADDRESS]), tmp->maclen);
	}
	/* Tunnels can have no LL_ADDR. RTA_PAYLOAD doesn't check it and try to
	 * dereference it in this manner
	 * #define RTA_PAYLOAD(rta) ((int)((rta)->rta_len) - RTA_LENGTH(0))
	 * */
	else {
	    tmp->maclen=0;
	}

	ipaddr_local_get(&tmp->linkaddrcount, &tmp->linkaddr, tmp->id, ainfo);
	ipaddr_global_get(&tmp->globaladdrcount, &tmp->globaladdr, tmp->id, ainfo);
    }

    release_nlmsg_list(linfo);
    release_nlmsg_list(ainfo);

    return head;
}

/**
 * returns local addresses for specified interface
 */
void ipaddr_local_get(int *count, char **bufPtr, int ifindex, struct nlmsg_list *ainfo) {
    int cnt=0;
    char * buf=0, * tmpbuf=0;
    char addr[16];
    struct rtattr * rta_tb[IFA_MAX+1];
    int pos;

    for ( ;ainfo ;  ainfo = ainfo->next) {
	struct nlmsghdr *n = &ainfo->h;
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	if ( (ifa->ifa_family == AF_INET6) && (ifa->ifa_index == ifindex) ) {
	    memset(rta_tb, 0, sizeof(rta_tb));
	    parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));
	    if (!rta_tb[IFA_LOCAL])   rta_tb[IFA_LOCAL]   = rta_tb[IFA_ADDRESS];
	    if (!rta_tb[IFA_ADDRESS]) rta_tb[IFA_ADDRESS] = rta_tb[IFA_LOCAL];
	    
	    memcpy(addr,(char*)RTA_DATA(rta_tb[IFLA_ADDRESS]),16);
	    if (addr[0]!=0xfe || addr[1]!=0x80) {
		continue; /* ignore non link-scoped addrs */
	    }
	    
	    /* ifa->ifa_flags & 128 - permenent */
	    /* printf("flags:%d : ",ifa->ifa_flags); */

	    pos = cnt*16;
	    buf = (char*) malloc( pos + 16);
	    memcpy(buf,tmpbuf, pos); /* copy old addrs */
	    memcpy(buf+pos,addr,16); /* copy new addr */
	    
	    free(tmpbuf);
	    tmpbuf = buf;
	    cnt++;
	}
    }
    *count = cnt;
    *bufPtr = buf;
}

/**
 * returns non-local addresses for specified interface
 */
void ipaddr_global_get(int *count, char **bufPtr, int ifindex, struct nlmsg_list *ainfo) {
    int cnt=0;
    char * buf=0, * tmpbuf=0;
    char addr[16];
    struct rtattr * rta_tb[IFA_MAX+1];
    int pos;

    for ( ;ainfo ;  ainfo = ainfo->next) {
	struct nlmsghdr *n = &ainfo->h;
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	if ( (ifa->ifa_family == AF_INET6) && (ifa->ifa_index == ifindex) ) {
	    memset(rta_tb, 0, sizeof(rta_tb));
	    parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));
	    if (!rta_tb[IFA_LOCAL])   rta_tb[IFA_LOCAL]   = rta_tb[IFA_ADDRESS];
	    if (!rta_tb[IFA_ADDRESS]) rta_tb[IFA_ADDRESS] = rta_tb[IFA_LOCAL];
	    
	    memcpy(addr,(char*)RTA_DATA(rta_tb[IFLA_ADDRESS]),16);
	    if ( (addr[0]==0xfe && addr[1]==0x80) || /* link local */
		 (addr[0]==0xff) ) { /* multicast */
		continue; /* ignore non link-scoped addrs */
	    }
	    
	    /* ifa->ifa_flags & 128 - permenent */
	    /* printf("flags:%d : ",ifa->ifa_flags); */

	    pos = cnt*16;
	    buf = (char*) malloc( pos + 16);
	    memcpy(buf,tmpbuf, pos); /* copy old addrs */
	    memcpy(buf+pos,addr,16); /* copy new addr */
	    
	    free(tmpbuf);
	    tmpbuf = buf;
	    cnt++;
	}
    }
    *count = cnt;
    *bufPtr = buf;
}

/** 
 * adds, updates or deletes addresses to interface
 * 
 * @param addr 
 * @param ifacename 
 * @param prefixLen 
 * @param preferred 
 * @param valid 
 * @param mode - 0-delete, 1-add, 2-update
 * 
 * @return 
 */
int ipaddr_add_or_del(const char * addr, const char *ifacename, int prefixLen, 
                      unsigned long preferred, unsigned long valid, int mode)
{
    struct rtnl_handle rth;
    struct {
	struct nlmsghdr 	n;
	struct ifaddrmsg 	ifa;
	char   			buf[256];
    } req;
    inet_prefix lcl;
    inet_prefix peer;
    int local_len = 0;
    int peer_len = 0;
    int scoped = 0;
    struct ifa_cacheinfo ci;

#ifdef LOWLEVEL_DEBUG
    printf("### iface=%s, addr=%s, add=%d ###\n", ifacename, addr, add);
#endif
    
    memset(&req, 0, sizeof(req));
    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
    switch (mode) {
    case ADDROPER_DEL:
        req.n.nlmsg_type = RTM_DELADDR; /* del address */
        req.n.nlmsg_flags = NLM_F_REQUEST;
        break;
    case ADDROPER_ADD:
        req.n.nlmsg_type = RTM_NEWADDR; /* add address */
        req.n.nlmsg_flags = NLM_F_REQUEST |NLM_F_CREATE|NLM_F_EXCL;
        break;
    case ADDROPER_UPDATE:
        req.n.nlmsg_type = RTM_NEWADDR; /* update address */
        req.n.nlmsg_flags = NLM_F_REQUEST | NLM_F_REPLACE;
        break;
    }
    req.ifa.ifa_family = AF_INET6;
    req.ifa.ifa_flags = 0;
    req.ifa.ifa_prefixlen = prefixLen;
    
    get_prefix_1(&lcl, (char*)addr, AF_INET6);
    
    addattr_l(&req.n, sizeof(req), IFA_LOCAL, &lcl.data, lcl.bytelen);
    local_len = lcl.bytelen;

    memset(&ci, 0, sizeof(ci));
    ci.ifa_valid = valid;
    ci.ifa_prefered = preferred;
    addattr_l(&req.n, sizeof(req), IFA_CACHEINFO, &ci, sizeof(ci));
    
    if (peer_len == 0 && local_len) {
	peer = lcl;
	addattr_l(&req.n, sizeof(req), IFA_ADDRESS, &lcl.data, lcl.bytelen);
    }
    if (req.ifa.ifa_prefixlen == 0)
	req.ifa.ifa_prefixlen = lcl.bitlen;
    
    if (!scoped)
	req.ifa.ifa_scope = default_scope(&lcl);
    
    rtnl_open(&rth, 0);
    ll_init_map(&rth);
    
    /* is there an interface with this ifindex? */
    if ((req.ifa.ifa_index = ll_name_to_index((char*)ifacename)) == 0) {
	sprintf(Message, "Cannot find device: %s", ifacename);
	return LOWLEVEL_ERROR_UNSPEC;
    }
    rtnl_talk(&rth, &req.n, 0, 0, NULL, NULL, NULL); fflush(stdout);

    return LOWLEVEL_NO_ERROR;
}

int ipaddr_add(const char * ifacename, int ifaceid, const char * addr, unsigned long pref,
	       unsigned long valid, int prefixLength)
{
    return ipaddr_add_or_del(addr,ifacename, prefixLength, pref, valid, ADDROPER_ADD);
}

int ipaddr_update(const char* ifacename, int ifindex, const char* addr,
		  unsigned long pref, unsigned long valid, int prefixLength)
{
    /* FIXME: Linux kernel currently does not provide API for dynamic adresses */

    return ipaddr_add_or_del(addr, ifacename, prefixLength, pref, valid, ADDROPER_UPDATE);

    return LOWLEVEL_NO_ERROR;
}


int ipaddr_del(const char * ifacename, int ifaceid, const char * addr, int prefixLength)
{
    return ipaddr_add_or_del(addr,ifacename, prefixLength, 0/*pref*/, 0/*valid*/, ADDROPER_DEL);
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
	
    /* Set the options  to receivce ipv6 traffic */
    if (setsockopt(Insock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on)) < 0) {
	sprintf(Message, "Unable to set up socket option IPV6_RECVPKTINFO.");
	return LOWLEVEL_ERROR_SOCK_OPTS;
    }

    if (thisifaceonly) {
	if (setsockopt(Insock, SOL_SOCKET, SO_BINDTODEVICE, ifacename, strlen(ifacename)+1) <0) {
	    sprintf(Message, "Unable to bind socket to interface %s.", ifacename);
	    return LOWLEVEL_ERROR_BIND_IFACE;
	}
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
	if (setsockopt(Insock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq6, sizeof(mreq6))) {
	    sprintf(Message, "error joining ipv6 group");
	    return LOWLEVEL_ERROR_MCAST_MEMBERSHIP;
	}
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
    char buf[256];
    char packed1[16];
    char packed2[16];
    struct rtattr * rta_tb[IFA_MAX+1];
    struct nlmsg_list *ainfo = NULL;
    struct nlmsg_list *head = NULL;
    struct rtnl_handle rth;

    int tentative = LOWLEVEL_TENTATIVE_DONT_KNOW;

    inet_pton6(addr,packed1);

    rtnl_open(&rth, 0);

    /* 2nd attribute: AF_UNSPEC, AF_INET, AF_INET6 */
    /* rtnl_wilddump_request(&rth, AF_PACKET, RTM_GETLINK); */
    rtnl_wilddump_request(&rth, AF_INET6, RTM_GETADDR);
    rtnl_dump_filter(&rth, store_nlmsg, &ainfo, NULL, NULL);

    head = ainfo;
    while (ainfo) {
	struct nlmsghdr *n = &ainfo->h;
	struct ifaddrmsg *ifa = NLMSG_DATA(n);
	
	memset(rta_tb, 0, sizeof(rta_tb));
	
	if (ifa->ifa_index == iface && ifa->ifa_family==AF_INET6) {
	    parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));
	    if (!rta_tb[IFA_LOCAL])   rta_tb[IFA_LOCAL]   = rta_tb[IFA_ADDRESS];
	    if (!rta_tb[IFA_ADDRESS]) rta_tb[IFA_ADDRESS] = rta_tb[IFA_LOCAL];
	    
	    inet_ntop6(RTA_DATA(rta_tb[IFA_LOCAL]), buf /*, sizeof(buf)*/);
	    memcpy(packed2,RTA_DATA(rta_tb[IFA_LOCAL]),16);

	    /* print_packed(packed1); printf(" "); print_packed(packed2); printf("\n"); */

	    /* is this addr which are we looking for? */
	    if (!memcmp(packed1,packed2,16) ) {
		if (ifa->ifa_flags & IFA_F_TENTATIVE)
		    tentative = LOWLEVEL_TENTATIVE_YES;
		else
		    tentative = LOWLEVEL_TENTATIVE_NO;
	    }
	}
	ainfo = ainfo->next;
    }

    /* now delete list */
    while (head) {
	ainfo = head;
	head = head->next;
	free(ainfo);
    }
    
    rtnl_close(&rth);

    return tentative;
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


/* iproute.c dummy link section */
int show_stats = 0; /* to disable iproute.c messages */
int preferred_family = AF_INET6;
char* rtnl_rtntype_n2a(int id, char *buf, int len) { return 0;}
char* rtnl_dsfield_n2a(int id, char *buf, int len) { return 0;}
char* rtnl_rttable_n2a(int id, char *buf, int len) { return 0;}
char* rtnl_rtprot_n2a(int id, char *buf, int len)  { return 0;}
char* rtnl_rtscope_n2a(int id, char *buf, int len) { return 0;}
char* rtnl_rtrealm_n2a(int id, char *buf, int len) { return 0;}

int rtnl_rtprot_a2n(__u32 *id, char *arg)  { return 0; }
int rtnl_rtscope_a2n(__u32 *id, char *arg) { return 0; }
int rtnl_rttable_a2n(__u32 *id, char *arg) { return 0; }
int rtnl_rtrealm_a2n(__u32 *id, char *arg) { return 0; }
int rtnl_dsfield_a2n(__u32 *id, char *arg) { return 0; }
int rtnl_rtntype_a2n(int *id, char *arg)   { return 0; }
int get_rt_realms(__u32 *realms, char *arg){ return 0; }
int default_scope(inet_prefix *lcl)        { return 0; }
char * _SL_; 

int dnet_pton(int af, const char *src, void *addr) { return 0; }
const char *dnet_ntop(int af, const void *addr, char *str, size_t len){ return 0; }
const char *ipx_ntop(int af, const void *addr, char *str, size_t len) { return 0; }

#ifdef DEBUG
#include <execinfo.h>

/*
  [Thomson: very clever function. It prints backtrace. Done by Michal Kowalczuk.]
 */ 
void print_trace(void)
{
  void *array[10];
  size_t size;

  size = backtrace (array, 10);
  backtrace_symbols_fd (array, size, 1);
}
#endif
