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
    TSrvOptIA_PD(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt, TMsg* parent);
    TSrvOptIA_PD(char * buf, int bufsize, TMsg* parent);
    TSrvOptIA_PD(uint32_t IAID, uint32_t T1, uint32_t T2, TMsg* parent);
    TSrvOptIA_PD(uint32_t IAID, uint32_t T1, uint32_t T2, int Code,
                 const std::string& Msg, TMsg* parent);

    void releaseAllPrefixes(bool quiet);

    void solicitRequest(SPtr<TSrvMsg> clientMsg, SPtr<TSrvOptIA_PD> queryOpt,
                        SPtr<TSrvCfgIface> ptr, bool fake);
    void renew         (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void rebind        (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void release       (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void confirm       (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    void decline       (SPtr<TSrvOptIA_PD> queryOpt, SPtr<TSrvCfgIface> iface);
    bool doDuties();
 private:
    SPtr<TIPv6Addr> ClntAddr;
    SPtr<TDUID> ClntDuid;

    /// @todo: replace with Parent->getIface();
    int Iface;

    bool existingLease();
    bool assignPrefix(SPtr<TSrvMsg> clientMsg, SPtr<TIPv6Addr> hint, bool fake);
    bool assignFixedLease(SPtr<TSrvOptIA_PD> request);

    List(TIPv6Addr) getFreePrefixes(SPtr<TSrvMsg> clientMsg, SPtr<TIPv6Addr> hint);

    uint32_t Prefered;
    uint32_t Valid;
    unsigned long PDLength;

};

#endif
