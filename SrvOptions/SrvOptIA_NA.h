#ifndef SRVOPTIA_NA_H
#define SRVOPTIA_NA_H

#include "OptIA_NA.h"
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

    TSrvOptIA_NA(SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > clntClasses,
		 long *clntFreeAddr,
		 long  &totalFreeAddr,
		 SmartPtr<TContainer<SmartPtr<TIPv6Addr> > > assignedAddresses,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 char &preference,
		 SmartPtr<TDUID> DUID, SmartPtr<TIPv6Addr> clntAddr, int iface,
		 int msgType, TMsg* parent);
    
    unsigned long countFreeAddrsForClient(SmartPtr<TSrvCfgMgr> cfgMgr,
					  SmartPtr<TSrvAddrMgr> addrMgr,
					  SmartPtr<TDUID> duid,
					  SmartPtr<TIPv6Addr> clntAddr,int iface);

    void solicit(SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr,
		 SmartPtr<TDUID> duid,
		 int iface,  unsigned long &addrCount);
    void request(SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr,
		 SmartPtr<TDUID> duid,
		 int iface, unsigned long &addrCount);

    void renew(SmartPtr<TSrvCfgMgr> cfgMgr,
	       SmartPtr<TSrvAddrMgr> addrMgr,
	       SmartPtr<TSrvOptIA_NA> queryOpt,
	       SmartPtr<TIPv6Addr> clntAddr,
	       SmartPtr<TDUID> duid,
	       int iface,  unsigned long &addrCount);

    void rebind(SmartPtr<TSrvCfgMgr> cfgMgr,
		SmartPtr<TSrvAddrMgr> addrMgr,
		SmartPtr<TSrvOptIA_NA> queryOpt,
		SmartPtr<TIPv6Addr> clntAddr,
		SmartPtr<TDUID> duid,
		int iface,  unsigned long &addrCount);

    void release(SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr,
		 SmartPtr<TDUID> duid,
		 int iface, unsigned long &addrCount);

    void confirm(SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr,
		 SmartPtr<TDUID> duid,
		 int iface, unsigned long &addrCount);
    
    void decline(SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvOptIA_NA> queryOpt,
		 SmartPtr<TIPv6Addr> clntAddr,
		 SmartPtr<TDUID> duid,
		 int iface, unsigned long &addrCount);
       
    bool doDuties();
 private:
    bool isAddrIn(SmartPtr<TIPv6Addr> addr,
		  SmartPtr<TContainer<SmartPtr<TIPv6Addr> > > addrLst);
};




#endif
