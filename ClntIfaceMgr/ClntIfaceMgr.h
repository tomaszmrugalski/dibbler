/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.h,v 1.2 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TClntIfaceMgr;
#ifndef CLNTIFACEMGR_H
#define CLNTIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "ClntTransMgr.h"
#include "ClntIfaceIface.h"
#include "IPv6Addr.h"
#include "Msg.h"

class TClntIfaceMgr : public TIfaceMgr
{
 public:
    TClntIfaceMgr();
    ~TClntIfaceMgr();
    
    bool sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr);
    
    bool sendMulticast(int iface, char *msg, int msgsize);
    
    SmartPtr<TMsg> select(unsigned int timeout);

    void setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
		  SmartPtr<TClntTransMgr> clntTransMgr,
		  SmartPtr<TClntCfgMgr>   clntCfgMgr,
		  SmartPtr<TClntAddrMgr>  clntAddrMgr);

    // --- option related ---
    void setDNSServerLst(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> srv, List(TIPv6Addr) addrs);
    void addOptDomains(List(char) domains);      // Domains
    void addOptNTPServers(List(TIPv6Addr) addr); // NTP servers
    void addOptTimezone(char * timezone);        // timezone: DEPRECIATED
    void addOptSIPServers(List(TIPv6Addr) addr); // SIP servers
    void addOptSIPDomains(List(char) domains);   // SIP domains
    void addOptFQDN(char * fqdn, char opts);     // FQDN

    void removeAllOpts();

  private:
    SmartPtr<TClntCfgMgr> ClntCfgMgr;
    SmartPtr<TClntAddrMgr> ClntAddrMgr;
    SmartPtr<TClntTransMgr> ClntTransMgr;
    SmartPtr<TClntIfaceMgr> That;
};

#endif 
