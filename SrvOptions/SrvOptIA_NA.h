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
    TSrvOptIA_NA(long IAID, long T1, long T2, int Code, string Msg, TMsg* parent);
    /* Constructor used in answers to:
    * - SOLICIT 
    * - SOLICIT (with RAPID_COMMIT)
    * - REQUEST */
    TSrvOptIA_NA(SPtr<TSrvOptIA_NA> queryOpt, SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,  int iface, int msgType, TMsg* parent);
    /// @todo: Why 5 constructors? 2 should be enough

    void releaseAllAddrs(bool quiet);

    void solicit(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void request(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    bool renew(SPtr<TSrvOptIA_NA> queryOpt, bool complainIfMissing);
    void rebind(SPtr<TSrvOptIA_NA> queryOpt,  unsigned long &addrCount);
    void release(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void confirm(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void decline(SPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    bool doDuties();
 private:
    SPtr<TIPv6Addr>   ClntAddr;
    SPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    SPtr<TSrvOptIAAddress> assignAddr(SPtr<TIPv6Addr> hint, unsigned long pref,
					  unsigned long valid, bool quiet);
    SPtr<TIPv6Addr> getFreeAddr(SPtr<TIPv6Addr> hint);
    SPtr<TIPv6Addr> getExceptionAddr();
};

#endif
