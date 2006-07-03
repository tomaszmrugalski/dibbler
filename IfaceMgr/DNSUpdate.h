/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DNSUpdate.h,v 1.1 2006-07-03 18:08:57 thomson Exp $
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif

#include "poslib.h"

/* return values of method run*/

#define DNSUPDATE_SUCCESS	      0
#define DNSUPDATE_ERROR	          1
#define DNSUPDATE_CONNFAIL        2
#define DNSUPDATE_SRVNOTAUTH      3

class DNSUpdate {
    
private:
    DnsMessage *message;
    _addr server;
    char* hostname;
    char* hostip;
    domainname* zoneroot;
    char* ttl;
    
    void createSOAMsg();
    void addinMsg_newRR();
    void addinMsg_delOldRR();
    bool DnsRR_avail(DnsMessage *msg, DnsRR& RemoteDnsRR);
    DnsRR* get_oldDnsRR();
    void sendMsg();
    
 public:
    DNSUpdate(char* dns_address,char*zonename,char* hostname,char* hostip,char* ttl, int mode);
    ~DNSUpdate();
    u_int run();
};
