#ifndef SRVOPTIA_NA_H
#define SRVOPTIA_NA_H

#include "OptIA_NA.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIAAddress.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"
#include "Container.h"
#include "IPv6Addr.h"

class TOptIA_NA;

class TSrvOptIA_NA : public TOptIA_NA
{
  public:
    
    TSrvOptIA_NA( SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr, SmartPtr<TDUID> duid,
		 int iface, unsigned long &addrCount, int msgType , TMsg* parent);

    TSrvOptIA_NA( char * buf, int bufsize, TMsg* parent);    
    TSrvOptIA_NA( long IAID, long T1, long T2, TMsg* parent);    
    TSrvOptIA_NA( long IAID, long T1, long T2, int Code, string Msg, TMsg* parent);    
    
/* Constructor used in answers to:
 * - SOLICIT 
 * - SOLICIT (with RAPID_COMMIT)
 * - REQUEST */
    TSrvOptIA_NA(SmartPtr<TSrvAddrMgr> addrMgr,  SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, 
		 bool doNotAssignAddrs,
		 int iface, int msgType, TMsg* parent);
    
    void releaseAllAddrs();

    void solicit(SmartPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void request(SmartPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void renew(SmartPtr<TSrvOptIA_NA> queryOpt,   unsigned long &addrCount);
    void rebind(SmartPtr<TSrvOptIA_NA> queryOpt,  unsigned long &addrCount);
    void release(SmartPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void confirm(SmartPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    void decline(SmartPtr<TSrvOptIA_NA> queryOpt, unsigned long &addrCount);
    bool doDuties();
 private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TIPv6Addr>   ClntAddr;
    SmartPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    SmartPtr<TSrvOptIAAddress> assignAddr(SmartPtr<TIPv6Addr> hint, unsigned long pref,
					  unsigned long valid, bool doNotAssignAddrs);
    SmartPtr<TIPv6Addr> getFreeAddr(SmartPtr<TIPv6Addr> hint);
    unsigned long countFreeAddrsForClient();
};




#endif
