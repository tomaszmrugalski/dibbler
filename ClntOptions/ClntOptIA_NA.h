#ifndef CLNTIA_NA_H_HEADER_INCLUDED_C112064B
#define CLNTIA_NA_H_HEADER_INCLUDED_C112064B

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
    TClntOptIA_NA(SmartPtr<TClntCfgIA> ClntCfgIA, TMsg* parent);
    TClntOptIA_NA(SmartPtr<TAddrIA> AddrIA, TMsg* parent);
    TClntOptIA_NA(SmartPtr<TAddrIA> clntAddrIA, bool zeroTimes, TMsg* parent);

    TClntOptIA_NA( char * buf, int bufsize, TMsg* parent);
    ~TClntOptIA_NA();    

    bool doDuties();
    int getStatusCode();
    void setThats(SmartPtr<TClntIfaceMgr> ifaceMgr, 
		  SmartPtr<TClntTransMgr> transMgr, 
		  SmartPtr<TClntCfgMgr>   cfgMgr, 
		  SmartPtr<TClntAddrMgr>  addrMgr,
		  SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, int iface);

    SmartPtr<TClntOptIAAddress> getAddr();
    SmartPtr<TClntOptIAAddress> getAddr(SmartPtr<TIPv6Addr> addr);
    void firstAddr();
    int countAddr();
    TClntOptIA_NA(SmartPtr<TClntCfgIA> ClntCfgIA, SmartPtr<TAddrIA> ClntaddrIA, TMsg* parent);
    bool isValid();

 private:
    void releaseAddr(long IAID, SmartPtr<TIPv6Addr> addr );
    int countValidAddrs(SmartPtr<TAddrIA> ptrAddrIA);

    SmartPtr<TIPv6Addr> Addr;
    bool Unicast;
    SmartPtr<TDUID> DUID;
    int Iface;

    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntTransMgr> TransMgr;
    SmartPtr<TClntCfgMgr> CfgMgr;
    SmartPtr<TClntAddrMgr> AddrMgr;
};


#endif /* IA_NA_H_HEADER_INCLUDED_C112064B */
