/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntIfaceMgr.h,v 1.9 2007-01-03 01:27:02 thomson Exp $
 *
 */

class TClntIfaceMgr;
class TClntMsg;
class TClntIfaceIface;
#ifndef CLNTIFACEMGR_H
#define CLNTIFACEMGR_H

#include "SmartPtr.h"
#include "IfaceMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "ClntTransMgr.h"
#include "ClntIfaceIface.h"
#include "IPv6Addr.h"
#include "ClntMsg.h"

class TClntIfaceMgr : public TIfaceMgr
{
 public:
    TClntIfaceMgr(string xmlFile);
    ~TClntIfaceMgr();
    friend ostream & operator <<(ostream & strum, TClntIfaceMgr &x);
    void dump();
    
    bool sendUnicast(int iface, char *msg, int size, SmartPtr<TIPv6Addr> addr);
    
    bool sendMulticast(int iface, char *msg, int msgsize);
    
    SmartPtr<TClntMsg> select(unsigned int timeout);

    void setThats(SmartPtr<TClntIfaceMgr> clntIfaceMgr,
		  SmartPtr<TClntTransMgr> clntTransMgr,
		  SmartPtr<TClntCfgMgr>   clntCfgMgr,
		  SmartPtr<TClntAddrMgr>  clntAddrMgr);

    bool fqdnAdd(SmartPtr<TClntIfaceIface> iface, string domainname);
    bool fqdnDel(SmartPtr<TClntIfaceIface> iface, SmartPtr<TAddrIA> ia, string domainname);

    bool addPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen, unsigned int pref, unsigned int valid);
    bool delPrefix(int iface, SPtr<TIPv6Addr> prefix, int prefixLen);

    // --- option related ---
    void removeAllOpts();

    unsigned int getTimeout();
    
    bool doDuties();

  private:
    string XmlFile;
    SmartPtr<TClntCfgMgr> ClntCfgMgr;
    SmartPtr<TClntAddrMgr> ClntAddrMgr;
    SmartPtr<TClntTransMgr> ClntTransMgr;
    SmartPtr<TClntIfaceMgr> That;
};

#endif 
