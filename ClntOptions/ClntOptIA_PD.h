/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTOPTIA_PD_H
#define CLNTOPTIA_PD_H

#include "OptIA_PD.h"
#include "ClntOptIAPrefix.h"
#include "ClntIfaceMgr.h"
#include "ClntTransMgr.h"
#include "ClntCfgMgr.h"
#include "ClntAddrMgr.h"
#include "IPv6Addr.h"

class TOptIA_PD;

class TClntOptIA_PD : public TOptIA_PD
{
 public:
    TClntOptIA_PD(SmartPtr<TClntCfgPD> ClntCfgPD, TMsg* parent);
    TClntOptIA_PD(SmartPtr<TAddrIA> clntAddrPD, TMsg* parent);
    TClntOptIA_PD(char * buf, int bufsize, TMsg* parent);
    ~TClntOptIA_PD();

    bool doDuties();
    int getStatusCode();
    void setContext(SPtr<TClntIfaceMgr> ifaceMgr, 
		    SPtr<TClntTransMgr> transMgr, 
		    SPtr<TClntCfgMgr>   cfgMgr, 
		    SPtr<TClntAddrMgr>  addrMgr,
		    SPtr<TDUID> srvDuid, SmartPtr<TIPv6Addr> srvAddr, TMsg* originalMsg);
    void setIface(int iface);

    SmartPtr<TClntOptIAPrefix> getPrefix();
    SmartPtr<TClntOptIAPrefix> getPrefix(SmartPtr<TIPv6Addr> prefix);
    void firstPrefix();
    int countPrefix();
    bool isValid();

    bool addPrefixes();
    bool updatePrefixes();
    bool delPrefixes();
 private:
    bool modifyPrefixes(PrefixModifyMode mode);
    void setState(EState state);
    void clearContext();

    SmartPtr<TIPv6Addr> Prefix;
    bool Unicast;
    SmartPtr<TDUID> DUID;
    int Iface;

    /* Context */
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    SmartPtr<TClntTransMgr> TransMgr;
    SmartPtr<TClntCfgMgr>   CfgMgr;
    SmartPtr<TClntAddrMgr>  AddrMgr;
    TMsg * OriginalMsg;
    /* Context */
};

#endif /* CLNTOPTIA_PD_H */
