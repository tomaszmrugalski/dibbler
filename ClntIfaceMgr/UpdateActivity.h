/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 *
 * released under GNU GPL v2 licence
 *
 * $Id: UpdateActivity.h,v 1.2 2006-03-03 20:47:17 thomson Exp $
 *
 */

#include "poslib.h"

/* return values of method run*/

#define SUCCESS	        0
#define ERROR	        1
#define CONNFAIL        2
#define SRVNOTAUTH      3

class UpdateActivity {
    
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
    UpdateActivity(char* dns_address,char*zonename,char* hostname,char* hostip,char* ttl);
    ~UpdateActivity();
    u_int run();
};
