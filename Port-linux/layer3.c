/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: layer3.c,v 1.12 2004-09-05 15:27:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2004/09/05 10:45:16  thomson
 * Socket binding fixed.
 *
 * Revision 1.10  2004/07/05 23:04:08  thomson
 * *** empty log message ***
 *
 * Revision 1.9  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.8  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.7  2004/07/01 18:13:51  thomson
 * Sockets are now bound to specific interface (bug #46)
 *
 * Revision 1.6  2004/06/04 20:54:34  thomson
 * *** empty log message ***
 *
 * Revision 1.5  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
 * Revision 1.4  2004/05/23 15:19:29  thomson
 * All comments translated to english.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <fnmatch.h>
#include <time.h>

#include "libnetlink.h"
#include "ll_map.h"
#include "utils.h"
#include "rt_names.h"
#include "Portable.h"

struct nlmsg_list
{
	struct nlmsg_list *next;
	struct nlmsghdr	  h;
};

int print_linkinfo(struct nlmsghdr *n);
int print_addrinfo(struct nlmsghdr *n);
int print_selected_addrinfo(int ifindex, struct nlmsg_list *ainfo);
void ipaddr_local_get(int *count, char **buf, int ifindex, struct nlmsg_list *ainfo);
void print_link_flags( unsigned flags);
int default_scope(inet_prefix *lcl);

int store_nlmsg(struct sockaddr_nl *who, struct nlmsghdr *n, void *arg)
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

    // required to display information about interface
    struct ifinfomsg *ifi;
    struct rtattr * tb[IFLA_MAX+1];
    int len;
    memset(tb, 0, sizeof(tb));

    rtnl_open(&rth, 0);
    rtnl_wilddump_request(&rth, preferred_family, RTM_GETLINK);
    rtnl_dump_filter(&rth, store_nlmsg, &linfo, NULL, NULL);
    
    // 2nd attribute: AF_UNSPEC, AF_INET, AF_INET6
    rtnl_wilddump_request(&rth, AF_UNSPEC, RTM_GETADDR);
    rtnl_dump_filter(&rth, store_nlmsg, &ainfo, NULL, NULL);

    // build list with interface names
    for (l=linfo; l; l = l->next) {
	// n = &l->h;
	ifi = NLMSG_DATA(&l->h);
	len = (&l->h)->nlmsg_len;
	len -= NLMSG_LENGTH(sizeof(*ifi));
	parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

//	printf("C: %d %s (flags:%d)\n",ifi->ifi_index,(char*)RTA_DATA(tb[IFLA_IFNAME]),ifi->ifi_flags);

	tmp = malloc(sizeof(struct iface));
	snprintf(tmp->name,MAX_IFNAME_LENGTH,(char*)RTA_DATA(tb[IFLA_IFNAME]));
	tmp->id=ifi->ifi_index;
	tmp->flags=ifi->ifi_flags;
	tmp->next=head;
	head=tmp;
//	printf("C: [%s,%d,%d]\n",tmp->name,tmp->id,tmp->flags);

	{
	    // This stuff reads MAC addr
	    tmp->maclen = RTA_PAYLOAD(tb[IFLA_ADDRESS]);
	    memset(tmp->mac,0,255);
	    memcpy(tmp->mac,RTA_DATA(tb[IFLA_ADDRESS]), tmp->maclen);
	}

	ipaddr_local_get(&tmp->linkaddrcount, &tmp->linkaddr, tmp->id, ainfo);
			 
	//tmp->linkaddrcount=1;
	//tmp->linkaddr = malloc(16);
	//memset(tmp->linkaddr,0,16);
    }

    release_nlmsg_list(linfo);
    release_nlmsg_list(ainfo);

    return head;
}

/*
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
		continue; // ignore non link-scoped addrs
	    }
	    
	    // ifa->ifa_flags & 128 - permenent
	    //printf("flags:%d : ",ifa->ifa_flags);

	    pos = cnt*16;
	    buf = (char*) malloc( pos + 16);
	    memcpy(buf,tmpbuf, pos); // copy old addrs
	    memcpy(buf+pos,addr,16); // copy new addr
	    
	    free(tmpbuf);
	    tmpbuf = buf;
	    cnt++;
	}
    }
    *count = cnt;
    *bufPtr = buf;
}

/*
 * adds or deletes addresses to interface
 */

int ipaddr_add_or_del(char * addr, char *ifacename,int add)
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

	memset(&req, 0, sizeof(req));
	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
	req.n.nlmsg_flags = NLM_F_REQUEST;
	if (add==1) req.n.nlmsg_type = RTM_NEWADDR; /* add address */
	else req.n.nlmsg_type = RTM_DELADDR;        /* del address */
	req.ifa.ifa_family = AF_INET6;
	req.ifa.ifa_flags = 0;

	get_prefix_1(&lcl, addr, AF_INET6);

	addattr_l(&req.n, sizeof(req), IFA_LOCAL, &lcl.data, lcl.bytelen);
	local_len = lcl.bytelen;

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

	// is there an interface with this ifindex?
	if ((req.ifa.ifa_index = ll_name_to_index(ifacename)) == 0) {
	    //fprintf(stderr, "Cannot find device \"%s\"\n", ifacename);
	    return -1;
	}
	rtnl_talk(&rth, &req.n, 0, 0, NULL, NULL, NULL); fflush(stdout);
	return 0;
}

int ipaddr_add(char * ifacename, int ifaceid, char * addr, long pref, long valid)
{
    return ipaddr_add_or_del(addr,ifacename,1);
}

int ipaddr_del(char * ifacename, int ifaceid, char * addr)
{
    return ipaddr_add_or_del(addr,ifacename,0);
}

int sock_add(char * ifacename,int ifaceid, char * addr, int port, int thisifaceonly)
{
    int error;
    int on = 1;
    struct addrinfo hints;
    struct addrinfo *res, *res2;
    struct ipv6_mreq mreq6; 
    int Insock;
    int multicast;
    char port_char[6];
    sprintf(port_char,"%d",port);
    char * tmp;

    if (!strncasecmp(addr,"ff",2))
	multicast = 1;
    else
	multicast = 0;

    //printf("### iface: %s(id=%d), addr=%s, port=%d, ifaceonly=%d mcast=%d ###\n",
    //ifacename,ifaceid, addr, port, thisifaceonly, multicast);
    //fflush(stdout);
    
    // Open a socket for inbound traffic
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    if( (error = getaddrinfo(NULL,  port_char, &hints, &res)) ){
	// getaddrinfo failed. Is IPv6 protocol supported by kernel?
	return -7;
    }
    if( (Insock = socket(AF_INET6, SOCK_DGRAM,0 )) < 0){
	//printf("socket creation failed. Is IPv6 protocol supported by kernel?\n");
	return -1;
    }	
	
    // Set the options  to receivce ipv6 traffic
    if (setsockopt(Insock, IPPROTO_IPV6, IPV6_PKTINFO, &on, sizeof(on)) < 0) {
	// Unable to set up socket option IPV6_PKTINFO
	return -8;
    }

    if (thisifaceonly) {
	if (setsockopt(Insock, SOL_SOCKET, SO_BINDTODEVICE, ifacename, strlen(ifacename)+1) <0) {
	    // Unable to bind to interface
	    return -10;
	}
    }

    // allow address reuse (this option sucks - why allow running multiple servers?)
    if (setsockopt(Insock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
	// Unable to set up socket option SO_REUSEADDR
	return -9;
    }

    // bind socket to a specified port
    struct sockaddr_in6 bindme;
    bzero(&bindme, sizeof(struct sockaddr_in6));
    bindme.sin6_family = AF_INET6;
    bindme.sin6_port   = htons(547);
    tmp = (char*)(&bindme.sin6_addr);
    inet_pton6(addr, tmp);
    //if (bind(Insock, res->ai_addr, res->ai_addrlen) < 0) {
    if (bind(Insock, (struct sockaddr*)&bindme, sizeof(bindme)) < 0) {
	// Unable to bind socket
	return -4;
    }

    freeaddrinfo(res);

    // multicast server stuff
    if (multicast) {
	hints.ai_flags = 0;
	if((error = getaddrinfo(addr, port_char, &hints, &res2))){
	    return -5;
	}
	memset(&mreq6, 0, sizeof(mreq6));
	mreq6.ipv6mr_interface = ifaceid;
	memcpy(&mreq6.ipv6mr_multiaddr, &((struct sockaddr_in6 *)res2->ai_addr)->sin6_addr,
	       sizeof(mreq6.ipv6mr_multiaddr));
	
	// Add to the all agent multicast address 
	if (setsockopt(Insock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq6, sizeof(mreq6))) {
	    //printf("Server: Error joining ipv6 group");
	    return -6;
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
	int lim=8;
	char cport[10];
	sprintf(cport,"%d",port);
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	if (getaddrinfo(addr, cport, &hints, &res) < 0) {
	    return -1; // Error in transmitting
	}

	if (setsockopt(sock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &lim, sizeof(lim)) < 0) {
	    return -2; // Error setting up socket
	}
 
	result = sendto(sock, buf, message_len, 0, res->ai_addr, res->ai_addrlen);
	freeaddrinfo(res);
	return result;
}

/*
 *
 */
int sock_recv(int fd, char * myPlainAddr, char * peerPlainAddr, char * buf, int buflen)
{
    struct msghdr msg;            // message received by recvmsg
    struct sockaddr_in6 peerAddr; // sender address
    struct sockaddr_in6 myAddr;   // my address
    struct iovec iov;             // simple structure containing buffer address and length

    struct cmsghdr *cm;           // control message
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
	return -1;
    }

    // get source address
    inet_ntop6((void*)&peerAddr.sin6_addr, peerPlainAddr);

    // get destination address
    for(cm = (struct cmsghdr *) CMSG_FIRSTHDR(&msg); cm; cm = (struct cmsghdr *) CMSG_NXTHDR(&msg, cm)){
	if (cm->cmsg_level != IPPROTO_IPV6 || cm->cmsg_type != IPV6_PKTINFO)
	    continue;
	pktinfo= (struct in6_pktinfo *) (CMSG_DATA(cm));
	inet_ntop6((void*)&pktinfo->ipi6_addr, myPlainAddr);
    }
    return result;

#if 0
    char * znak;
    int port;
    struct sockaddr_storage from;
    socklen_t fromlen = sizeof(from);
    char addr_char[NI_MAXHOST], port_char[NI_MAXSERV];
    char iface_name[MAX_IFNAME_LENGTH];


    result = recvfrom( fd, buf, buflen, 0, (struct sockaddr *) &from,&fromlen);

    getnameinfo((struct sockaddr *)&from, fromlen,
		addr_char, sizeof(addr_char), port_char, sizeof(port_char), 
		NI_NUMERICHOST|NI_NUMERICSERV);

    if ( (znak = strchr(addr_char,37)) ) { // split after % sign
	*znak=0;
	strncpy(iface_name,++znak,MAX_IFNAME_LENGTH);
    } else {
	iface_name[0]=0;
    }
    strcpy(addr,addr_char);
    port = atoi(port_char);
    // result - received bytes count (or -1 if something is wrong)
    // addr_char - address (in plain text)
    // port_char - port (in plain text)
    // port      - port (integer)
    //printf("sock_recv2(): result=%d, addr=%s, iface=%s port %d\n",result,addr_char,iface_name,port);
    return result;
#endif
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

    int tentative=-1;

    inet_pton6(addr,packed1);

    rtnl_open(&rth, 0);

    // 2nd attribute: AF_UNSPEC, AF_INET, AF_INET6
    // rtnl_wilddump_request(&rth, AF_PACKET, RTM_GETLINK);
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

	    // print_packed(packed1); printf(" "); print_packed(packed2); printf("\n");

	    // is this addr which are we looking for?
	    if (!memcmp(packed1,packed2,16) ) {
		if (ifa->ifa_flags & IFA_F_TENTATIVE)
		    tentative = 1;
		else
		    tentative = 0;
	    }
	}
	ainfo = ainfo->next;
    }

    // now delete list
    while (head) {
	ainfo = head;
	head = head->next;
	free(ainfo);
    }
    
    // FIXME: close rth socket

    return tentative;
}



/***************************************************************
 *** DEPRECIATED ***********************************************
 ***************************************************************/

/* 
 * wy¶wietla flagi interfejsu
 */
void print_link_flags( unsigned flags)
{
	printf("<");
	if (flags & IFF_RUNNING)     { flags &= ~IFF_RUNNING; printf("RUNNING "); }
	if (flags & IFF_LOOPBACK)    { flags &=~IFF_LOOPBACK; printf("LOOPBACK "); }
	if (flags & IFF_BROADCAST)   { flags &=~IFF_BROADCAST; printf("BROADCAST "); }
	if (flags & IFF_POINTOPOINT) { flags &=~IFF_POINTOPOINT; printf("POINTOPOINT "); }
	if (flags & IFF_MULTICAST)   { flags &=~IFF_MULTICAST; printf("MULTICAST "); }
	if (flags & IFF_NOARP)    { flags &=~IFF_NOARP;    printf("NOARP "); }
	if (flags & IFF_ALLMULTI) { flags &=~IFF_ALLMULTI; printf("ALLMULTI "); }
	if (flags & IFF_PROMISC) { flags &=~IFF_PROMISC; printf("PROMISC "); }
	if (flags & IFF_MASTER) { flags &=~IFF_MASTER; printf("MASTER "); }
	if (flags & IFF_SLAVE) { flags &=~IFF_SLAVE; printf("SLAVE "); }
	if (flags & IFF_DEBUG) { flags &=~IFF_DEBUG; printf("DEBUG "); }
	if (flags & IFF_DYNAMIC) { flags &=~IFF_DYNAMIC; printf("DYNAMIC "); }
	if (flags & IFF_AUTOMEDIA) { flags &=~IFF_AUTOMEDIA; printf("AUTOMEDIA "); }
	if (flags & IFF_PORTSEL) { flags &=~IFF_PORTSEL; printf("PORTSEL "); }
	if (flags & IFF_NOTRAILERS) { flags &=~IFF_NOTRAILERS; printf("NOTRAILERS "); }
	if (flags & IFF_UP) { flags &=~IFF_UP; printf("UP"); }
	printf("> ");
}

int if_show()
{
    struct nlmsg_list *linfo = NULL;
    struct nlmsg_list *ainfo = NULL;
    struct nlmsg_list *l;
    struct rtnl_handle rth;
    int preferred_family = AF_PACKET;
//    char *filter_dev = NULL;
    
//    ipaddr_reset_filter(oneline);
    
//    if (filter.family == AF_UNSPEC)
//	filter.family = preferred_family;
//    filter_dev="eth0";
    
    rtnl_open(&rth, 0);
    rtnl_wilddump_request(&rth, preferred_family, RTM_GETLINK);
    rtnl_dump_filter(&rth, store_nlmsg, &linfo, NULL, NULL);
    
//    filter.ifindex = ll_name_to_index(filter_dev);
    
    // 2 atrybut: AF_UNSPEC, AF_INET, AF_INET6
    rtnl_wilddump_request(&rth, AF_UNSPEC, RTM_GETADDR);
    rtnl_dump_filter(&rth, store_nlmsg, &ainfo, NULL, NULL);

    // wy¶wietl informacje o interfejsach
    for (l=linfo; l; l = l->next) {
	struct ifinfomsg *ifi = NLMSG_DATA(&l->h);

	// wy¶wietl info o interfejsie
	print_linkinfo(&l->h);

	// wy¶wietl adresy dla tego interfejsu
	print_selected_addrinfo(ifi->ifi_index, ainfo);
    }
    return 0;
}

int print_selected_addrinfo(int ifindex, struct nlmsg_list *ainfo)
{
	for ( ;ainfo ;  ainfo = ainfo->next) {
		struct nlmsghdr *n = &ainfo->h;
		struct ifaddrmsg *ifa = NLMSG_DATA(n);

		if (ifa->ifa_index == ifindex) print_addrinfo(n);
	}
	return 0;
}


int print_linkinfo(struct nlmsghdr *n)
{
	struct ifinfomsg *ifi = NLMSG_DATA(n);
	struct rtattr * tb[IFLA_MAX+1];
	int len = n->nlmsg_len;

	len -= NLMSG_LENGTH(sizeof(*ifi));
	memset(tb, 0, sizeof(tb));
	parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

	// interface number + interface name
	printf("%d: %4s", ifi->ifi_index,
		tb[IFLA_IFNAME] ? (char*)RTA_DATA(tb[IFLA_IFNAME]) : "<nil>");

	// What physical interface is this interface on?
	if (tb[IFLA_LINK]) {
		SPRINT_BUF(b1);
		int iflink = *(int*)RTA_DATA(tb[IFLA_LINK]);
		if (iflink == 0)
			printf("@NONE: ");
		else {
			printf("@%-4s: ", ll_idx_n2a(iflink, b1));
		}
	} else {
		printf(":      ");
	}

	{
	    char b1[SPRINT_BSIZE];
	    printf(" link/%-8s ", ll_type_n2a(ifi->ifi_type, b1, sizeof(b1)));
	}

	// flags <UP,NOARP,LOOPBACK,MULTICAST,BROADCAST>
	print_link_flags(ifi->ifi_flags);
	
	printf("\n");
	return 0;
}

int print_linkinfo2(struct nlmsghdr *n)
{
    struct ifinfomsg *ifi;
    struct rtattr * tb[IFLA_MAX+1];
    static int i=1;
    int len;

    ifi=NLMSG_DATA(n);
    len = n->nlmsg_len;
    len -= NLMSG_LENGTH(sizeof(*ifi));
    memset(tb, 0, sizeof(tb));
    parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);
    printf("Interface no %d: %s \n", i++, (char*)RTA_DATA(tb[IFLA_IFNAME]));
    return 0;
}

int print_addrinfo(struct nlmsghdr *n)
{
    struct ifaddrmsg *ifa = NLMSG_DATA(n);
    struct rtattr * rta_tb[IFA_MAX+1];
    char abuf[256];
    printf("    ");
    
    memset(rta_tb, 0, sizeof(rta_tb));
    parse_rtattr(rta_tb, IFA_MAX, IFA_RTA(ifa), n->nlmsg_len - NLMSG_LENGTH(sizeof(*ifa)));
    
    if (!rta_tb[IFA_LOCAL])   rta_tb[IFA_LOCAL]   = rta_tb[IFA_ADDRESS];
    if (!rta_tb[IFA_ADDRESS]) rta_tb[IFA_ADDRESS] = rta_tb[IFA_LOCAL];
    
    // rodzina
    if (ifa->ifa_family == AF_INET6) printf("inet6 "); else
	if (ifa->ifa_family == AF_INET) printf("inet  "); else
	    printf("other: ");
    
    // adres
    printf("%s", rt_addr_n2a(ifa->ifa_family,
			     RTA_PAYLOAD(rta_tb[IFA_LOCAL]),
			     RTA_DATA(rta_tb[IFA_LOCAL]),
			     abuf, sizeof(abuf)));
    
    // maska
    printf("/%d", ifa->ifa_prefixlen);
    
    // anycast?
    if (rta_tb[IFA_ANYCAST]) {
	printf("any %s ",
	       rt_addr_n2a(ifa->ifa_family,
			   RTA_PAYLOAD(rta_tb[IFA_ANYCAST]),
			   RTA_DATA(rta_tb[IFA_ANYCAST]),
			   abuf, sizeof(abuf)));
    }

    // scope?
    // printf(" scope %s ", rtnl_rtscope_n2a(ifa->ifa_scope, b1, sizeof(b1)));
    
    if (!(ifa->ifa_flags&IFA_F_PERMANENT)) {
	printf("dynamic ");
    } 
    
    // dynamic address
    if (rta_tb[IFA_CACHEINFO]) {
	struct ifa_cacheinfo *ci = RTA_DATA(rta_tb[IFA_CACHEINFO]);
	char buf[128];
	if (ci->ifa_valid == 0xFFFFFFFFU)
	    sprintf(buf, "valid_lft forever");
	else
	    sprintf(buf, "valid_lft %dsec", ci->ifa_valid);
	if (ci->ifa_prefered == 0xFFFFFFFFU)
	    sprintf(buf+strlen(buf), " preferred_lft forever");
	else
	    sprintf(buf+strlen(buf), " preferred_lft %dsec", ci->ifa_prefered);
	printf("       %s", buf);
    }
    printf("\n");
    return 0;
}
