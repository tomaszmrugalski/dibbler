/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntParsIfaceOpt.h,v 1.6 2004-11-30 00:42:50 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2004/11/29 21:21:56  thomson
 * Client parser now supports 'option lifetime' directive (bug #75)
 *
 * Revision 1.4  2004/10/25 20:45:52  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.3  2004/10/02 13:11:24  thomson
 * Boolean options in config file now can be specified with YES/NO/TRUE/FALSE.
 * Unicast communication now can be enable on client side (disabled by default).
 *
 * Revision 1.2  2004/05/23 22:37:54  thomson
 * *** empty log message ***
 *
 *                                                                           
 */

#ifndef TPARSEIFACEOPT_H
#define TPARSEIFACEOPT_H

#include "DHCPConst.h"
#include "Container.h"

#include "ClntParsIAOpt.h"
#include "StationID.h"
#include "IPv6Addr.h"

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
    bool isNewGroup();
    void setNewGroup(bool newGr);
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

private:
    bool NewGroup;//indicates whether new group should be created
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
};


#endif
