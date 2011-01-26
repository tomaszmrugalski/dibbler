/*
 * Dibbler - a portable DHCPv6
 *
 * author: Paul Schauer <p.schauer(at)comcast(dot)net>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: lowlevel-macos.c,v 1.3 2009-07-22 20:19:08 thomson Exp $
 *
 * Based on Port-linux/lowlevel-linux.c
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

#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>

#include <net/if_dl.h>

#include "Portable.h"

#define LOWLEVEL_DEBUG 1

char Message[1024] = {0};

int lowlevelInit() {
    return LOWLEVEL_NO_ERROR;
}

int lowlevelExit() {
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
struct iface * if_list_get() {

    /*
     * Translating between Mac OS X internal representation of link and IP address
     * and Dibbler internal format.
     */

    struct ifaddrs *ifList, *ifTemp;
    struct iface *ifRtnList = NULL;
    struct iface *ifRtnTemp = NULL;
    struct sockaddr_dl *linkInfo;
    char *myV6array;
    int addrPos = 0;
    int tmpInt = 0, tmpInt1 = 0;

    if (getifaddrs(&ifList) != 0) {
        perror("Error in getifaddrs: ");
        return ifRtnList;
    }

    ifTemp = malloc(sizeof (struct ifaddrs));
    ifRtnList = malloc(sizeof (struct iface));

    /* If there is at least one interface, populate first element before loop */

    if (ifList != NULL) {
        strlcpy(ifRtnList->name, ifList->ifa_name, strlen(ifList->ifa_name) + 1);
        ifRtnList->flags = ifList->ifa_flags;
    }
    ifRtnList->next = NULL;

    /* First pass through entire ifList: collect unique interface names and flags */
    for (ifTemp = ifList; ifTemp != NULL; ifTemp = ifTemp->ifa_next) {
        if (strncmp(ifRtnList->name, ifTemp->ifa_name, strlen(ifTemp->ifa_name))
                != 0) {
            ifRtnTemp = malloc(sizeof (struct iface));
            strlcpy(ifRtnTemp->name, ifTemp->ifa_name, strlen(ifTemp->ifa_name)
                    + 1);
            ifRtnTemp->flags = ifTemp->ifa_flags;
            ifRtnTemp->next = ifRtnList;
            ifRtnList = ifRtnTemp;
        }
    }

    /*
     * Second pass through ifList: collect link and IP layer info for each interface
     * by name
     */

    myV6array = malloc(16);
    for (ifTemp = ifList; ifTemp != NULL; ifTemp = ifTemp->ifa_next) {
        for (ifRtnTemp = ifRtnList; ifRtnTemp != NULL; ifRtnTemp
                = ifRtnTemp->next) {
            if (strncmp(ifRtnTemp->name, ifTemp->ifa_name, strlen(
                    ifTemp->ifa_name)) == 0) {
                if (ifTemp->ifa_addr->sa_family == AF_INET6) {

                    memcpy(
                            myV6array,
                            &((struct sockaddr_in6 *) ifTemp->ifa_addr)->sin6_addr,
                            16);

                    if (myV6array[0] == 0xfe && myV6array[1] == 0x80) {
                        /*
                         * IPv6 address is link local
                         */
                        if (!ifRtnTemp->linkaddr) {
                            ifRtnTemp->linkaddrcount = 0;
                            ifRtnTemp->linkaddr = malloc(16);
                        }

                        addrPos = ifRtnTemp->linkaddrcount * 16;
                        memcpy(ifRtnTemp->linkaddr + addrPos, myV6array, 16);

                        ifRtnTemp->linkaddrcount++;

                    } else {
                        /*
                         *  IPv6 address is global
                         */

                        if (!ifRtnTemp->globaladdr) {
                            ifRtnTemp->globaladdrcount = 0;
                            ifRtnTemp->globaladdr = malloc(16);
                        }
                        addrPos = ifRtnTemp->globaladdrcount * 16;
                        memcpy(ifRtnTemp->globaladdr + addrPos, myV6array, 16);
                        ifRtnTemp->globaladdrcount++;

                    }

                } else if (ifList->ifa_addr->sa_family == AF_LINK) {
                    linkInfo = (struct sockaddr_dl *) ifTemp->ifa_addr;
                    ifRtnTemp->id = linkInfo->sdl_index;
                    /*
                     * NOTE!
                     * sdl_type is unsigned character; hardwareType is
                     * integer
                     */
                    ifRtnTemp->hardwareType = linkInfo->sdl_type;
                    if (linkInfo->sdl_alen > 1) {
                        memcpy(ifRtnTemp->mac, LLADDR(linkInfo),
                                linkInfo->sdl_alen);
                        ifRtnTemp->maclen = linkInfo->sdl_alen;
                    }
                }
            }
        }
    }
    /* Print out ifRtnList data if debug mode */
#ifdef LOWLEVEL_DEBUG {
    for (ifRtnTemp = ifRtnList; ifRtnTemp != NULL; ifRtnTemp
            = ifRtnTemp->next) {
        printf("Interface name: %s\n", ifRtnTemp->name);
        printf("\tInterface index: %i\n", ifRtnTemp->id);
        printf("\tInterface type: %x\n", ifRtnTemp->hardwareType);
        printf("\tLink layer Length: %x\n", ifRtnTemp->maclen);
        printf("\tLink layer Addr: ");
        for (tmpInt1 = 0; tmpInt1 < ifRtnTemp->maclen - 1; tmpInt1++) {
            printf("%02x:", (unsigned char) ifRtnTemp->mac[tmpInt1]);
        }
        printf("%02x", (unsigned char) ifRtnTemp->mac[tmpInt1++]);
        printf("\n");
        printf("\t%s Local IPv6 address count: %i\n", ifRtnTemp->name,
                ifRtnTemp->linkaddrcount);
        printf("\tLocal IPv6 address(es):\n");
        tmpInt = 0;
        for (tmpInt1 = 0; tmpInt1 < ifRtnTemp->linkaddrcount; tmpInt1++) {
            printf("\t%i=", tmpInt1);
            for (tmpInt = 0; tmpInt < 14; tmpInt += 2) {
                printf("%02x%02x:", (unsigned char) ifRtnTemp->linkaddr[tmpInt + tmpInt1 * 16], (unsigned char) ifRtnTemp->linkaddr[tmpInt + 1
                        + tmpInt1 * 16]);
            }
            printf("%02x%02x", (unsigned char) ifRtnTemp->linkaddr[tmpInt
                    + tmpInt1 * 16], (unsigned char) ifRtnTemp->linkaddr[tmpInt + 1
                    + tmpInt1 * 16]);
            printf("\n");
        }
        printf("\t%s Global IPv6 address count: %i\n", ifRtnTemp->name,
                ifRtnTemp->globaladdrcount);
        printf("\tGlobal IPv6 address(es):\n");
        tmpInt = 0;
        for (tmpInt1 = 0; tmpInt1 < ifRtnTemp->globaladdrcount; tmpInt1++) {
            printf("\t%i=", tmpInt1);
            for (tmpInt = 0; tmpInt < 14; tmpInt += 2) {
                printf("%02x%02x:", (unsigned char) ifRtnTemp->globaladdr[tmpInt
                        + tmpInt1 * 16], (unsigned char) ifRtnTemp->globaladdr[tmpInt + 1
                        + tmpInt1 * 16]);
            }
            printf("%02x%02x", (unsigned char) ifRtnTemp->globaladdr[tmpInt
                    + tmpInt1 * 16], (unsigned char) ifRtnTemp->globaladdr[tmpInt + 1
                    + tmpInt1 * 16]);
            printf("\n");
        }
        printf("\tInterface flags: %x\n", ifRtnTemp->flags);
    }
    fflush(stdout);
#endif

    return ifRtnList;
} /* end of if_list_get */

int ipaddr_add(const char * ifacename, int ifaceid, const char * addr,
        unsigned long pref, unsigned long valid, int prefixLength) {
    /* TODO: implement this */
#ifdef LOWLEVEL_DEBUG
    printf("### In ipaddr_add\n###");
    fflush(stdout);
#endif
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

int ipaddr_update(const char* ifacename, int ifindex, const char* addr,
        unsigned long pref, unsigned long valid, int prefixLength) {
    /* TODO: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

int ipaddr_del(const char * ifacename, int ifaceid, const char * addr,
        int prefixLength) {
    /* TODO: implement this */
    return LOWLEVEL_ERROR_NOT_IMPLEMENTED;
}

int sock_add(char * ifacename, int ifaceid, char * addr, int port,
        int thisifaceonly, int reuse) {
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
    sprintf(port_char, "%d", port);

    if (!strncasecmp(addr, "ff", 2))
        multicast = 1;
    else
        multicast = 0;

#ifdef LOWLEVEL_DEBUG
    printf(
            "### iface: %s(id=%d), addr=%s, port=%d, ifaceonly=%d reuse=%d###\n",
            ifacename, ifaceid, addr, port, thisifaceonly, reuse);
    fflush(stdout);
#endif

    /* Open a socket for inbound traffic */
    memset(&hints, 0, sizeof (hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    if ((error = getaddrinfo(NULL, port_char, &hints, &res))) {
        sprintf(Message,
                "getaddrinfo failed. Is IPv6 protocol supported by kernel?");
        return LOWLEVEL_ERROR_GETADDRINFO;
    }
    if ((Insock = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        sprintf(Message,
                "socket creation failed. Is IPv6 protocol supported by kernel?");
        return LOWLEVEL_ERROR_UNSPEC;
    }

#if 0
    /* this part looks like Linux only code */
    /* Set the options  to receive ipv6 traffic */
    if (setsockopt(Insock, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof (on)) < 0) {
        sprintf(Message, "Unable to set up socket option IPV6_RECVPKTINFO.");
        return LOWLEVEL_ERROR_SOCK_OPTS;
    }
#endif

    if (thisifaceonly) {
#if 0
        /* this part also looks like linux only code */
        if (setsockopt(Insock, SOL_SOCKET, SO_BINDTODEVICE, ifacename, strlen(ifacename) + 1) < 0) {
            sprintf(Message, "Unable to bind socket to interface %s.", ifacename);
            return LOWLEVEL_ERROR_BIND_IFACE;
        }
#endif
    }

    /* allow address reuse (this option sucks - why allow running multiple servers?) */
    if (reuse != 0) {
        if (setsockopt(Insock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on)) < 0) {
            sprintf(Message, "Unable to set up socket option SO_REUSEADDR.");
            return LOWLEVEL_ERROR_REUSE_FAILED;
        }
    }

    /* bind socket to a specified port */
    bzero(&bindme, sizeof (struct sockaddr_in6));
    bindme.sin6_family = AF_INET6;
    bindme.sin6_port = htons(port);
    /* Bind to interface using scope_id */ 
    bindme.sin6_scope_id = ifaceid;
    tmp = (char*) (&bindme.sin6_addr);
    inet_pton6(addr, tmp);
    if (bind(Insock, (struct sockaddr*) & bindme, sizeof (bindme)) < 0) {
        sprintf(Message, "Unable to bind socket: %s", strerror(errno));
        return LOWLEVEL_ERROR_BIND_FAILED;
    }

    freeaddrinfo(res);

    /* multicast server stuff */
    if (multicast) {
        hints.ai_flags = 0;
        if ((error = getaddrinfo(addr, port_char, &hints, &res2))) {
            sprintf(Message, "Failed to obtain getaddrinfo");
            return LOWLEVEL_ERROR_GETADDRINFO;
        }
        memset(&mreq6, 0, sizeof (mreq6));
        mreq6.ipv6mr_interface = ifaceid;
        memcpy(&mreq6.ipv6mr_multiaddr,
                &((struct sockaddr_in6 *) res2->ai_addr)->sin6_addr,
                sizeof (mreq6.ipv6mr_multiaddr));

        /* Add to the all agent multicast address */
#if 0
        /* there seem to be on IPV6_ADD_MEMBERSHIP, only IP_ADD_MEMBERSHIP. Will it work? */
        if (setsockopt(Insock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq6, sizeof (mreq6))) {
            sprintf(Message, "error joining ipv6 group");
            return LOWLEVEL_ERROR_MCAST_MEMBERSHIP;
        }
#endif
        freeaddrinfo(res2);
    }

    return Insock;
}

int sock_del(int fd) {
    return close(fd);
}

int sock_send(int sock, char *addr, char *buf, int message_len, int port,
        int iface) {
    struct addrinfo hints, *res;
    int result;
    char cport[10];
    sprintf(cport, "%d", port);

    memset(&hints, 0, sizeof (hints));
    hints.ai_family = PF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    if (getaddrinfo(addr, cport, &hints, &res) < 0) {
        return -1; /* Error in transmitting */
    }

    result = sendto(sock, buf, message_len, 0, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    if (result < 0) {
        sprintf(Message, "Unable to send data (dst addr: %s)", addr);
        return LOWLEVEL_ERROR_SOCKET;
    }
    return LOWLEVEL_NO_ERROR;
}

/*
 *
 */
int sock_recv(int fd, char * myPlainAddr, char * peerPlainAddr, char * buf,
        int buflen) {
    struct msghdr msg; /* message received by recvmsg */
    struct sockaddr_in6 peerAddr; /* sender address */
    struct sockaddr_in6 myAddr; /* my address */
    struct iovec iov; /* simple structure containing buffer address and length */

    struct cmsghdr *cm; /* control message */
    struct in6_pktinfo *pktinfo;

    char control[CMSG_SPACE(sizeof (struct in6_pktinfo))];
    char controlLen = CMSG_SPACE(sizeof (struct in6_pktinfo));
    int result = 0;
    bzero(&msg, sizeof (msg));
    bzero(&peerAddr, sizeof (peerAddr));
    bzero(&myAddr, sizeof (myAddr));
    bzero(&control, sizeof (control));
    iov.iov_base = buf;
    iov.iov_len = buflen;

    msg.msg_name = &peerAddr;
    msg.msg_namelen = sizeof (peerAddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = controlLen;

    result = recvmsg(fd, &msg, 0);

    if (result == -1) {
        return LOWLEVEL_ERROR_UNSPEC;
    }

    /* get source address */
    inet_ntop6((void*) & peerAddr.sin6_addr, peerPlainAddr);

    /* get destination address */
    for (cm = (struct cmsghdr *) CMSG_FIRSTHDR(&msg); cm; cm = (struct cmsghdr *) CMSG_NXTHDR(&msg, cm)) {
        if (cm->cmsg_level != IPPROTO_IPV6 || cm->cmsg_type != IPV6_PKTINFO)
            continue;
        pktinfo = (struct in6_pktinfo *) (CMSG_DATA(cm));
        inet_ntop6((void*) & pktinfo->ipi6_addr, myPlainAddr);
    }
    return result;
}

void microsleep(int microsecs) {
    struct timespec x, y;

    x.tv_sec = (int) microsecs / 1000000;
    x.tv_nsec = (microsecs - x.tv_sec * 1000000) * 1000;
    nanosleep(&x, &y);
}

/*
 * returns: -1 - address not found, 0 - addr is ok, 1 - addr is tentative
 */
int is_addr_tentative(char * ifacename, int iface, char * addr) {
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

char * getAAAKeyFilename(uint32_t SPI) {
    static char filename[1024];
    if (SPI != 0)
        snprintf(filename, 1024, "%s%s%x", "/var/lib/dibbler/AAA/", "AAA-key-",
            SPI);
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
        if (!ret)
            break;
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

char * error_message() {
    return Message;
}


/** 
 * begin link monitoring
 * 
 * @param monitored_links head of the monitored links list
 * @param notify pointer to variable that is going to be modifed if change is detected
 */
void link_state_change_init(volatile struct link_state_notify_t * monitored_links, volatile int * notify)
{
    printf("Link change monitoring is not supported yet on Macs. Sorry.\n");
    return;
}

/** 
 * cleanup code for link state monitoring
 * 
 */
void link_state_change_cleanup()
{
    return;
}
