#ifndef SRVADDRMGR_H_HEADER_INCLUDED_C1008DAA
#define SRVADDRMGR_H_HEADER_INCLUDED_C1008DAA

#include "AddrMgr.h"
#include "SrvCfgAddrClass.h"
class TSrvAddrMgr : public TAddrMgr
{
  public:
    //## Wczytuje bazê danych przydzielonych adresów do pamiêci, odpowiednio j¹
    //## aktualizuj¹c tzn. usuwaj¹c przestarza³e adresy, ca³e IA lub informacje
    //## o kliencie.
    TSrvAddrMgr(string addrdb, bool loadDB=false);

    long getTimeout();
    bool addClntAddr(SmartPtr<TDUID> duid,long IAID, 
		     SmartPtr<TIPv6Addr> addr, long pref, long valid);
    unsigned long getAddrCount(SmartPtr<TDUID> duid, int iface);

    void doDuties();
    
    void getAddrsCount(
        SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > classes,
        long    *clntCnt,
        long    *addrCnt,
        SmartPtr<TDUID> duid, 
        int iface);

    bool addrIsFree(SmartPtr<TIPv6Addr> addr);
};

#endif
