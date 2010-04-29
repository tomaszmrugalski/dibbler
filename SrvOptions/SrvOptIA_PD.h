/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 * 
 * released under GNU GPL v2 only licence
 *
 *
 */

class TSrvOptIA_PD;

#ifndef SRVOPTIA_PD_H
#define SRVOPTIA_PD_H

#include "OptIA_PD.h"
#include "SrvOptIAPrefix.h" 
#include "SmartPtr.h"
#include "DUID.h"
#include "Container.h"
#include "IPv6Addr.h"
#include "SrvCfgIface.h"

class TSrvOptIA_PD : public TOptIA_PD
{
  public:
    
    TSrvOptIA_PD(SPtr<TSrvOptIA_PD> queryOpt,
		 SPtr<TIPv6Addr> clntAddr, SPtr<TDUID> duid,
		 int iface, int msgType , TMsg* parent);

    TSrvOptIA_PD(char * buf, int bufsize, TMsg* parent);    
    TSrvOptIA_PD(long IAID, long T1, long T2, TMsg* parent);    
    TSrvOptIA_PD(long IAID, long T1, long T2, int Code, string Msg, TMsg* parent);

    void releaseAllPrefixes(bool quiet);

    void solicitRequest(SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface, bool fake);
    void renew         (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void rebind        (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void release       (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void confirm       (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void decline       (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    bool doDuties();
 private:
    SPtr<TIPv6Addr>   ClntAddr;
    SPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    int assignPrefix(SPtr<TIPv6Addr> hint, bool quiet);
    List(TIPv6Addr) getFreePrefixes(SPtr<TIPv6Addr> hint);

    unsigned long Prefered;
    unsigned long Valid;
    unsigned long PDLength;

};

#endif
