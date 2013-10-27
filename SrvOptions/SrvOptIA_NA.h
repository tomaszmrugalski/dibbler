/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrvOptIA_NA;
#ifndef SRVOPTIA_NA_H
#define SRVOPTIA_NA_H

#include "OptIA_NA.h"
#include "SrvOptIAAddress.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "Container.h"
#include "IPv6Addr.h"
#include "SrvMsg.h"

class TSrvOptIA_NA : public TOptIA_NA
{
  public:
    
    TSrvOptIA_NA(SPtr<TSrvOptIA_NA> queryOpt, SPtr<TIPv6Addr> clntAddr, SPtr<TDUID> duid,
		 int iface, unsigned long &addrCount, int msgType , TMsg* parent);
    TSrvOptIA_NA(char * buf, int bufsize, TMsg* parent);    
    TSrvOptIA_NA(long IAID, long T1, long T2, TMsg* parent);    
    TSrvOptIA_NA(long IAID, long T1, long T2, int Code, const std::string& Msg, TMsg* parent);
    TSrvOptIA_NA(SPtr<TSrvOptIA_NA> queryOpt, SPtr<TSrvMsg> queryMsg, TMsg* parent);

    void releaseAllAddrs(bool quiet);

    void solicit(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void request(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    bool renew(SPtr<TSrvOptIA_NA> queryOpt, bool complainIfMissing);
    void rebind(SPtr<TSrvOptIA_NA> queryOpt,  unsigned long &addrCount);
    void release(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void decline(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    bool doDuties();
 private:
    bool assignCachedAddr(bool quiet);
    bool assignRequestedAddr(SPtr<TSrvMsg> queryMsg, SPtr<TSrvOptIA_NA> queryOpt, bool quiet);
    bool assignSequentialAddr(SPtr<TSrvMsg> clientMsg, bool quiet);
    bool assignRandomAddr(SPtr<TSrvMsg> queryMsg, bool quiet);
    SPtr<TIPv6Addr> getAddressHint(SPtr<TSrvMsg> clientReq, SPtr<TIPv6Addr> hint);
    bool assignAddr(SPtr<TIPv6Addr> addr, uint32_t pref, uint32_t valid, bool quiet);
    bool assignFixedLease(SPtr<TSrvOptIA_NA> req, bool quiet);

    SPtr<TIPv6Addr>   ClntAddr;
    SPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    SPtr<TIPv6Addr> getFreeAddr(SPtr<TIPv6Addr> hint);
    SPtr<TIPv6Addr> getExceptionAddr();
};

#endif
