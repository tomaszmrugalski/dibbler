/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
 *          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
 *          from ENSEEIHT, Toulouse, France
 *
 * released under GNU GPL v2 licence
 *
 * $Id: DNSUpdate.h,v 1.8 2006-10-04 21:43:56 thomson Exp $
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif

#include "poslib.h"


/* used in config. file */
enum DnsUpdateModeCfg {
    DNSUPDATE_MODE_NONE = 0,
    DNSUPDATE_MODE_PTR  = 1,
    DNSUPDATE_MODE_BOTH = 2
};

/* return values of method run*/
enum DnsUpdateResult {
    DNSUPDATE_SUCCESS=0,
    DNSUPDATE_ERROR=1,
    DNSUPDATE_CONNFAIL=2,
    DNSUPDATE_SRVNOTAUTH=3,
    DNSUPDATE_SKIP=4
};

/* used in DNSUpdate constructor */
enum DnsUpdateMode {
    DNSUPDATE_PTR=1,
    DNSUPDATE_PTR_CLEANUP=2,
    DNSUPDATE_AAAA=3,
    DNSUPDATE_AAAA_CLEANUP=4
};

class DNSUpdate {

 public:
    enum DnsUpdateProtocol {
	DNSUPDATE_TCP,
	DNSUPDATE_UDP,
	DNSUPDATE_ANY
    };

private:
    DnsMessage *message;
    _addr server;
    char* _hostname;
    char* hostip;
    domainname* zoneroot;
    char* ttl;
    DnsUpdateMode updateMode;
    DnsUpdateProtocol _proto;
   
    void splitHostDomain(string fqdnName);

    void createSOAMsg();
    void addinMsg_newPTR();
    void addinMsg_newAAAA();
    void addinMsg_delOldRR();
    void deleteAAAARecordFromRRSet();
    void deletePTRRecordFromRRSet();
    bool DnsRR_avail(DnsMessage *msg, DnsRR& RemoteDnsRR);
    DnsRR* get_oldDnsRR();
    void sendMsg(unsigned int timeout);
    void sendMsgTCP(unsigned int timeout);
    void sendMsgUDP(unsigned int timeout);
     
 public:
    DNSUpdate(string dns_address, string zonename, string hostname, 
	      string hostip, DnsUpdateMode updateMode, 
	      DnsUpdateProtocol proto = DNSUPDATE_TCP);
    ~DNSUpdate();
    DnsUpdateResult run(int timeout);
    void showResult(int result);
};
