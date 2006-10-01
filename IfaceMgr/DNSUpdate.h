/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DNSUpdate.h,v 1.7 2006-10-01 20:47:17 thomson Exp $
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif

#include "poslib.h"

/* return values of method run*/

enum DnsUpdateCfg {
    DNSUPDATE_NONE = 0,
    DNSUPDATE_PTR  = 1,
    DNSUPDATE_BOTH = 2
}

enum DnsUpdateResult {
    DNSUPDATE_SUCCESS=0,
    DNSUPDATE_ERROR=1,
    DNSUPDATE_CONNFAIL=2,
    DNSUPDATE_SRVNOTAUTH=3,
    DNSUPDATE_SKIP=4
};

enum DnsUpdateMode {
    DNSUPDATE_PTR=1,
    DNSUPDATE_PTR_CLEANUP=2,
    DNSUPDATE_AAAA=3,
    DNSUPDATE_AAAA_CLEANUP=4
};

class DNSUpdate {
    
private:
    DnsMessage *message;
    _addr server;
    char* hostname;
    char* hostip;
    domainname* zoneroot;
    char* ttl;
    DnsUpdateMode updateMode;
    
    void splitHostDomain(string fqdnName);

    void createSOAMsg();
    void addinMsg_newPTR();
    void addinMsg_newAAAA();
    void addinMsg_delOldRR();
    void deleteAAAARecordFromRRSet();
    void deletePTRRecordFromRRSet();
    bool DnsRR_avail(DnsMessage *msg, DnsRR& RemoteDnsRR);
    DnsRR* get_oldDnsRR();
    void sendMsg();
  
    
 public:
    DNSUpdate(string dns_address, string zonename, string hostname, string hostip,
	      DnsUpdateMode updateMode);
    ~DNSUpdate();
    DnsUpdateResult run();
    void showResult(int result);
};
