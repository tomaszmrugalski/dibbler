/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Grzegorz Pluto <g.pluto(at)u-r-b-a-n(dot)pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVADDRMGR_H
#define SRVADDRMGR_H

#include <vector>
#include "AddrMgr.h"
#include "SrvCfgAddrClass.h"
#include "SrvCfgPD.h"

#define SrvAddrMgr() (TSrvAddrMgr::instance())

class TSrvAddrMgr : public TAddrMgr
{
  public:
    static void instanceCreate(const std::string& xmlFile, bool loadDB);
    static TSrvAddrMgr & instance();

    class TSrvCacheEntry
    {
    public:
        TIAType type; // address or prefix
        SPtr<TIPv6Addr> Addr;  // cached address, previously assigned to a client
        SPtr<TDUID>     Duid;  // client's duid
    };

    struct TExpiredInfo
    {
        SPtr<TAddrClient> client;
        SPtr<TAddrIA> ia;
        SPtr<TIPv6Addr> addr; // address or prefix
	int prefixLen; // just for prefixes
    };

    ~TSrvAddrMgr();

    // IA address management
    bool addClntAddr(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
                     int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
                     SPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid,
                     bool quiet);
    bool delClntAddr(SPtr<TDUID> duid,unsigned long IAID, SPtr<TIPv6Addr> addr,
                     bool quiet);
    virtual bool verifyAddr(SPtr<TIPv6Addr> addr);

    // TA address management
    bool addTAAddr(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
                   int iface, unsigned long iaid, SPtr<TIPv6Addr> addr,
                   unsigned long pref, unsigned long valid);
    bool delTAAddr(SPtr<TDUID> duid,unsigned long iaid, SPtr<TIPv6Addr> addr, bool quiet);

    // prefix management
    virtual bool delPrefix(SPtr<TDUID> clntDuid, unsigned long IAID, SPtr<TIPv6Addr> prefix, bool quiet);
    virtual bool verifyPrefix(SPtr<TIPv6Addr> addr);

    // how many addresses does this client have?
    unsigned long getLeaseCount(SPtr<TDUID> duid);

    void doDuties(std::vector<TExpiredInfo>& addrLst,
                  std::vector<TExpiredInfo>& tempAddrLst,
                  std::vector<TExpiredInfo>& prefixLst);

    void getAddrsCount(SPtr<List(TSrvCfgAddrClass)> classes, long *clntCnt,
                       long *addrCnt, SPtr<TDUID> duid, int iface);

    bool addrIsFree(SPtr<TIPv6Addr> addr);
    bool taAddrIsFree(SPtr<TIPv6Addr> addr);

    SPtr<TIPv6Addr> getFirstAddr(SPtr<TDUID> clntDuid);

    // address and prefix caching
    SPtr<TIPv6Addr> getCachedEntry(SPtr<TDUID> clntDuid, TIAType type);
    bool delCachedEntry(SPtr<TIPv6Addr> cachedEntry, TIAType type);
    bool delCachedEntry(SPtr<TDUID> clntDuid, TIAType type);
    void addCachedEntry(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedEntry, TIAType type);

    void setCacheSize(int bytes);
    void dump();

 protected:
    void print(std::ostream & out);

    TSrvAddrMgr(const std::string& xmlfile, bool loadDB);
    static TSrvAddrMgr * Instance;

    void cacheRead();
    void cacheDump();
    void checkCacheSize();
    List(TSrvCacheEntry) Cache; // list of cached addresses
    size_t CacheMaxSize; // maximum number of cached elements
};

#endif
