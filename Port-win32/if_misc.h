#ifndef IF_MISC_H
#define IF_MISC_H

#ifndef MAX_IFNAME_LENGTH 

#ifdef WIN32
#define MAX_IFNAME_LENGTH 255
#endif

#ifdef _LINUX_
#define MAX_IFNAME_LENGTH 64
#endif

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
