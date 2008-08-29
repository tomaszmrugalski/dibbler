/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *   changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                      
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: ClntParsIfaceOpt.h,v 1.10 2008-08-29 00:07:27 thomson Exp $
 *
 */

class TClntParsIfaceOpt;
#ifndef CLNTPARSEIFACEOPT_H
#define CLNTPARSEIFACEOPT_H

#include "DHCPConst.h"
#include "Container.h"

#include "ClntParsIAOpt.h"
#include "StationID.h"
#include "IPv6Addr.h"
class TClntOptVendorSpec;

#include <iostream>
#include <string>
using namespace std;


class TClntParsIfaceOpt : public TClntParsIAOpt
{
 public:
    TClntParsIfaceOpt();
    ~TClntParsIfaceOpt();
    void setUnicast(bool unicast);
    bool getUnicast();
    bool getRapidCommit();
    void setRapidCommit(bool rapid);
/*     bool isNewGroup(); */
/*     void setNewGroup(bool newGr); */
    bool getIsIAs();
    void setIsIAs(bool state);
    
    //-- options related methods --
    // option: DNS Servers
    List(TIPv6Addr) * getDNSServerLst();
    void setDNSServerLst(List(TIPv6Addr) *lst);
    bool getReqDNSServer();

    // option: Domain
    List(string) * getDomainLst();
    void setDomainLst(List(string) * domains);
    bool getReqDomain();

    // option: NTP servers
    List(TIPv6Addr) * getNTPServerLst();
    void setNTPServerLst(List(TIPv6Addr) *lst);
    bool getReqNTPServer();

    // option: Timezone
    string getTimezone();
    void setTimezone(string timeZone);
    bool getReqTimezone();

    // option: SIP servers
    List(TIPv6Addr) * getSIPServerLst();
    void setSIPServerLst(List(TIPv6Addr) *addr);
    bool getReqSIPServer();

    // option: SIP domains
    List(string) * getSIPDomainLst();
    void setSIPDomainLst(List(string) *domainlist);
    bool getReqSIPDomain();

    // option: FQDN
    string getFQDN();
    void setFQDN(string fqdn);
    bool getReqFQDN();

    // option: NIS servers
    List(TIPv6Addr) * getNISServerLst();
    void setNISServerLst( List(TIPv6Addr) *nislist);
    bool getReqNISServer();

    // option: NIS+ servers
    List(TIPv6Addr) * getNISPServerLst();
    void setNISPServerLst( List(TIPv6Addr) *nisplist);
    bool getReqNISPServer();

    // option: NIS domain
    string getNISDomain();
    void setNISDomain(string domain);
    bool getReqNISDomain();

    // option: NISP domain
    string getNISPDomain();
    void setNISPDomain(string domain);
    bool getReqNISPDomain();

    // option: Lifetime
    bool getLifetime();
    void setLifetime();
    bool getReqLifetime();

    // option: Prefix Delegation
    void setPrefixDelegation();
    bool getReqPrefixDelegation();

    // option: vendor-spec
    void setVendorSpec(List(TClntOptVendorSpec) vendorSpec);
    void setVendorSpec();
    bool getReqVendorSpec();
    List(TClntOptVendorSpec) getVendorSpec();

private:
    bool NoIAs;
    bool Unicast; // do we accept unicast?
    bool RapidCommit;

    List(TIPv6Addr) DNSServerLst;
    List(string) DomainLst;
    List(TIPv6Addr) NTPServerLst;
    string Timezone;
    List(TIPv6Addr) SIPServerLst;
    List(string) SIPDomainLst;
    string FQDN;
    List(TIPv6Addr) NISServerLst;
    List(TIPv6Addr) NISPServerLst;
    string NISDomain;
    string NISPDomain;
    bool Lifetime;
    List(TClntOptVendorSpec) VendorSpec;

    bool ReqDNSServer;
    bool ReqDomain;
    bool ReqNTPServer;
    bool ReqTimezone;
    bool ReqSIPServer;
    bool ReqSIPDomain;
    bool ReqFQDN;
    bool ReqNISServer;
    bool ReqNISPServer;
    bool ReqNISDomain;
    bool ReqNISPDomain;
    bool ReqLifetime;
    bool ReqPrefixDelegation;
    bool ReqVendorSpec;
};


#endif
