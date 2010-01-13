/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */
#ifndef CLNTOPTIA_NA_H
#define CLNTOPTIA_NA_H

#include "ClntCfgIA.h"
#include "OptIA_NA.h"
#include "ClntOptIAAddress.h"
#include "ClntIfaceMgr.h"
#include "ClntTransMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "IPv6Addr.h"

class TOptIA_NA;

class TClntOptIA_NA : public TOptIA_NA
{
 public:
    TClntOptIA_NA(SPtr<TClntCfgIA> ClntCfgIA, TMsg* parent);
    TClntOptIA_NA(SPtr<TAddrIA> AddrIA, TMsg* parent);
    TClntOptIA_NA(SPtr<TAddrIA> clntAddrIA, bool zeroTimes, TMsg* parent);
    TClntOptIA_NA(char * buf, int bufsize, TMsg* parent);
    ~TClntOptIA_NA();    

    bool doDuties();
    int getStatusCode();
    void setContext(SPtr<TClntIfaceMgr> ifaceMgr, 
		    SPtr<TClntTransMgr> transMgr, 
		    SPtr<TClntCfgMgr>   cfgMgr, 
		    SPtr<TClntAddrMgr>  addrMgr,
		    SPtr<TDUID> srvDuid, SPtr<TIPv6Addr> srvAddr, int iface);
    void setIface(int iface);

    SPtr<TClntOptIAAddress> getAddr();
    SPtr<TClntOptIAAddress> getAddr(SPtr<TIPv6Addr> addr);
    void firstAddr();
    int countAddr();
    TClntOptIA_NA(SPtr<TClntCfgIA> ClntCfgIA, 
		  SPtr<TAddrIA> ClntaddrIA, 
		  TMsg* parent);
    bool isValid();

 private:
    void releaseAddr(long IAID, SPtr<TIPv6Addr> addr );
    int countValidAddrs(SPtr<TAddrIA> ptrAddrIA);

    SPtr<TIPv6Addr> Addr;
    bool Unicast;
    SPtr<TDUID> DUID;
    int Iface;

    /* Context */
    SPtr<TClntIfaceMgr> IfaceMgr;
    SPtr<TClntTransMgr> TransMgr;
    SPtr<TClntCfgMgr>   CfgMgr;
    SPtr<TClntAddrMgr>  AddrMgr;
};


#endif /* IA_NA_H_HEADER_INCLUDED_C112064B */
