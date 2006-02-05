/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptTA.h,v 1.1.2.1 2006-02-05 23:41:13 thomson Exp $
 *
 */

class TOptTA;
class TMsg;

#ifndef CLNTIA_TA_H
#define CLNTIA_TA_H

#include "OptTA.h"
#include "ClntOptIAAddress.h"
#include "ClntIfaceMgr.h"
#include "ClntTransMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "IPv6Addr.h"
#include "ClntCommon.h"

class TClntOptTA : public TOptTA
{
 public:
    TClntOptTA(unsigned int iaid, TMsg* parent);
    TClntOptTA(char * buf, int bufsize, TMsg* parent);
    ~TClntOptTA();    

    bool doDuties();
    int getStatusCode();

    SmartPtr<TClntOptIAAddress> getAddr();
    SmartPtr<TClntOptIAAddress> getAddr(SmartPtr<TIPv6Addr> addr);
    void firstAddr();
    int countAddr();
    bool isValid();
    void setIface(int iface); // used to override interface (e.g. when msg is received via loopback)

    void setContext(SmartPtr<TClntAddrMgr> addrMgr, SmartPtr<TClntIfaceMgr> ifaceMgr,
		    SmartPtr<TClntCfgMgr> cfgMgr, int iface, SmartPtr<TIPv6Addr> clntAddr);

 private:
    void releaseAddr(long IAID, SmartPtr<TIPv6Addr> addr );

    SmartPtr<TIPv6Addr> Addr;
    int Iface;

    TCtx * Ctx;
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
};


#endif /* CLNTIA_TA_H */
