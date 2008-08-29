/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvAddrMgr.h,v 1.11 2008-08-29 00:07:33 thomson Exp $
 *
 */

class TSrvAddrMgr;
class TSrvCacheEntry;
#ifndef SRVADDRMGR_H
#define SRVADDRMGR_H

#include "AddrMgr.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgPD.h"

class TSrvCacheEntry
{
 public:
    SPtr<TIPv6Addr> Addr;       // cached address, previously assigned to a client
    SPtr<TDUID>     Duid;       // client's duid    
};

class TSrvAddrMgr : public TAddrMgr
{
  public:
    TSrvAddrMgr(string xmlfile);
    ~TSrvAddrMgr();

    long getTimeout();
    // IA address management
    bool addClntAddr(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr,
		     int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
		     SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid,
		     bool quiet);
    bool delClntAddr(SmartPtr<TDUID> duid,unsigned long IAID, SmartPtr<TIPv6Addr> addr,
		     bool quiet);

    // TA address management
    bool addTAAddr(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr,
		   int iface, unsigned long iaid, SmartPtr<TIPv6Addr> addr, 
		   unsigned long pref, unsigned long valid);
    bool delTAAddr(SmartPtr<TDUID> duid,unsigned long iaid, SmartPtr<TIPv6Addr> addr);

    // how many addresses does this client have?
    unsigned long getAddrCount(SmartPtr<TDUID> duid);

    void doDuties();
    
    void getAddrsCount(SmartPtr<List(TSrvCfgAddrClass)> classes, long *clntCnt,
		       long *addrCnt, SmartPtr<TDUID> duid, int iface);

    bool addrIsFree(SmartPtr<TIPv6Addr> addr);
    bool taAddrIsFree(SmartPtr<TIPv6Addr> addr);

    SPtr<TIPv6Addr> getFirstAddr(SPtr<TDUID> clntDuid);

    // address caching
    SPtr<TIPv6Addr> getCachedAddr(SPtr<TDUID> clntDuid);
    bool delCachedAddr(SPtr<TIPv6Addr> addr);
    bool delCachedAddr(SPtr<TDUID> clntDuid);
    void addCachedAddr(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr);
    void setCacheSize(int bytes);
    void dump();

 protected:
    void print(ostream & out);

 private:
    void cacheRead();
    void cacheDump();
    void checkCacheSize();
    List(TSrvCacheEntry) Cache; // list of cached addresses
    int CacheMaxSize;           // maximum number of cached elements
};

#endif
