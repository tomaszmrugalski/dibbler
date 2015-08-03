/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof WNuk <keczi@poczta.onet.pl>
 *          Grzegorz Pluto <g.pluto(at)u-r-b-a-n(dot)pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <cstdlib>
#include "SrvAddrMgr.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include "SrvCfgAddrClass.h"
#include "Portable.h"
#include "SrvCfgMgr.h"

using namespace std;

TSrvAddrMgr * TSrvAddrMgr::Instance = 0;

TSrvAddrMgr::TSrvAddrMgr(const std::string& xmlfile, bool loadDB)
    :TAddrMgr(xmlfile, loadDB) {

    this->CacheMaxSize = 999999999;
    this->cacheRead();
}

TSrvAddrMgr::~TSrvAddrMgr() {
    Log(Debug) << "SrvAddrMgr cleanup." << LogEnd;
}

/**
 * @brief adds an address to the client.
 *
 * Adds a single address to the client. If the client is not present in
 * address manager, adds it, too. Also, if client's IA is missing, adds it as well.
 *
 * @param clntDuid client DUID
 * @param clntAddr client's address (link-local, used if unicast is to be used)
 * @param iface interface, on which client is reachable
 * @param IAID ID of an IA
 * @param T1 - T1 timer
 * @param T2 - T2 timer
 * @param addr - address to be added
 * @param pref preferred lifetime
 * @param valid valid lifetime
 * @param quiet quiet mode (e.g. used during SOLICIT handling, don't print anything about
 *              adding client/IA/address, as it will be deleted immediately anyway)
 *
 * @return true, if addition was successful
 */
bool TSrvAddrMgr::addClntAddr(SPtr<TDUID> clntDuid , SPtr<TIPv6Addr> clntAddr,
                              int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
                              SPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid,
                              bool quiet)
{
    // find config interface for this ifindex
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(iface);
    if (!cfgIface) {
        Log(Error) << "Failed to add address: config interface with ifindex=" << iface
                   << " not found." << LogEnd;
        return false;
    }

    // find this client
    SPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) )
            break;
    }

    // have we found this client?
    if (!ptrClient) {
        if (!quiet) Log(Debug) << "Adding client (DUID=" << clntDuid->getPlain()
                               << ") to addrDB." << LogEnd;
        ptrClient = new TAddrClient(clntDuid);
        this->addClient(ptrClient);
    }

    // find this IA
    SPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();
    while ( ptrIA = ptrClient->getIA() ) {
        if ( ptrIA->getIAID() == IAID)
            break;
    }

    // have we found this IA?
    if (!ptrIA) {
        ptrIA = new TAddrIA(cfgIface->getName(), iface, IATYPE_IA, clntAddr, clntDuid, T1, T2, IAID);
        ptrClient->addIA(ptrIA);
        if (!quiet)
            Log(Debug) << "Adding IA (IAID=" << IAID << ") to addrDB." << LogEnd;
    }

    SPtr <TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        if (*ptrAddr->get()==*addr)
            break;
    }

    // address already exists
    if (ptrAddr) {
        Log(Warning) << "Address " << *ptrAddr
                     << " is already assigned to this IA." << LogEnd;
        return false;
    }

    // add address
    ptrAddr = new TAddrAddr(addr, pref, valid);
    ptrIA->addAddr(ptrAddr);
    if (!quiet)
        Log(Debug) << "Adding " << ptrAddr->get()->getPlain()
                   << " to IA (IAID=" << IAID << ") to addrDB." << LogEnd;
    return true;
}

/// Frees address (also deletes IA and/or client, if this was last address)
///
/// @param clntDuid DUID of the client
/// @param IAID IA identifier
/// @param clntAddr address to be removed from DB
/// @param quiet print out anything?
///
/// @return true if removal was successful
bool TSrvAddrMgr::delClntAddr(SPtr<TDUID> clntDuid, unsigned long IAID,
                              SPtr<TIPv6Addr> clntAddr, bool quiet)
{

    // find this client
    SPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) )
            break;
    }
    if (!ptrClient) { // have we found this client?
        Log(Warning) << "Client (DUID=" << clntDuid->getPlain()
                     << ") not found in addrDB, cannot delete address and/or client." << LogEnd;
        return false;
    }

    // find this IA
    SPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();
    while ( ptrIA = ptrClient->getIA() ) {
        if ( ptrIA->getIAID() == IAID)
            break;
    }
    if (!ptrIA) { // have we found this IA?
        Log(Warning) << "IA (IAID=" << IAID << ") not assigned to client, cannot delete address and/or IA."
                     << LogEnd;
        return false;
    }

    // find an address
    SPtr <TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        if (*ptrAddr->get()==*clntAddr)
            break;
    }
    if (!ptrAddr) {
        Log(Warning) << "Address " << *clntAddr << " not assigned, cannot delete." << LogEnd;
        return false;
    }

    ptrIA->delAddr(clntAddr);
    this->addCachedEntry(clntDuid, clntAddr, IATYPE_IA);
    if (!quiet)
        Log(Debug) << "Deleted address " << *clntAddr << " from addrDB." << LogEnd;

    if (!ptrIA->countAddr()) {
        if (!quiet)
            Log(Debug) << "Deleted empty IA (IAID=" << IAID << ") from addrDB." << LogEnd;
        ptrClient->delIA(IAID);
    }

    if (!ptrClient->countIA() && !ptrClient->countTA() && !ptrClient->countPD()) {
        if (!quiet)
            Log(Debug) << "Deleted empty client (DUID=" << clntDuid->getPlain()
                       << ") from addrDB." << LogEnd;
        this->delClient(clntDuid);
    }
    return true;
}

/**
 * @brief adds TA address to AddrMgr
 *
 * adds temporary address. Add missing client or TA, if necessary.
 *
 * @param clntDuid
 * @param clntAddr
 * @param iface
 * @param iaid
 * @param addr
 * @param pref
 * @param valid
 *
 * @return
 */
bool TSrvAddrMgr::addTAAddr(SPtr<TDUID> clntDuid , SPtr<TIPv6Addr> clntAddr,
                            int iface, unsigned long iaid, SPtr<TIPv6Addr> addr,
                            unsigned long pref, unsigned long valid) {
    // find config interface for this ifindex
    SPtr<TSrvCfgIface> cfgIface = SrvCfgMgr().getIfaceByID(iface);
    if (!cfgIface) {
        Log(Error) << "Failed to add temp. address: config interface with ifindex=" << iface
                   << " not found." << LogEnd;
        return false;
    }

    // find this client
    SPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) )
            break;
    }

    // have we found this client?
    if (!ptrClient) {
        Log(Debug) << "Adding client (DUID=" << clntDuid->getPlain() << ") to the addrDB." << LogEnd;
        ptrClient = new TAddrClient(clntDuid);
        this->addClient(ptrClient);
    }

    // find this TA
    SPtr <TAddrIA> ta;
    ptrClient->firstTA();
    while ( ta = ptrClient->getTA() ) {
        if ( ta->getIAID() == iaid)
            break;
    }

    // have we found this TA?
    if (!ta) {
        ta = new TAddrIA(cfgIface->getName(), iface, IATYPE_TA, clntAddr, clntDuid,
                         DHCPV6_INFINITY, DHCPV6_INFINITY, iaid);
        ptrClient->addTA(ta);
        Log(Debug) << "Adding TA (IAID=" << iaid << ") to the addrDB." << LogEnd;
    }

    SPtr <TAddrAddr> ptrAddr;
    ta->firstAddr();
    while ( ptrAddr = ta->getAddr() ) {
        if (*ptrAddr->get()==*addr)
            break;
    }

    // address already exists
    if (ptrAddr) {
        Log(Warning) << "Address " << *ptrAddr << " is already assigned to TA (iaid="
                     << iaid << ")." << LogEnd;
        return false;
    }

    // add address
    ptrAddr = new TAddrAddr(addr, pref, valid);
    ta->addAddr(ptrAddr);
    Log(Debug) << "Adding " << ptrAddr->get()->getPlain() << " to TA (IAID=" << iaid
               << ") to addrDB." << LogEnd;
    return true;
}

/**
 * Frees address (also deletes IA and/or client, if this was last address)
 *
 * @param clntDuid DUID of the client
 * @param iaid IAID of IA that contains address to be deleted
 * @param clntAddr address to be deleted
 * @param quiet should method log deleted address?
 *
 * @return true if removal was successful
 */
bool TSrvAddrMgr::delTAAddr(SPtr<TDUID> clntDuid, unsigned long iaid,
                            SPtr<TIPv6Addr> clntAddr, bool quiet) {
    // find this client
    SPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) )
            break;
    }

    // have we found this client?
    if (!ptrClient) {
        Log(Warning) << "Client (DUID=" << clntDuid->getPlain()
                     << ") not found in addrDB, cannot delete address and/or client." << LogEnd;
        return false;
    }

    // find this IA
    SPtr <TAddrIA> ta;
    ptrClient->firstTA();
    while ( ta = ptrClient->getTA() ) {
        if ( ta->getIAID() == iaid)
            break;
    }

    // have we found this TA?
    if (!ta) {
        Log(Warning) << "TA (IAID=" << iaid << ") not assigned to client, cannot delete address and/or IA."
                     << LogEnd;
        return false;
    }

    SPtr <TAddrAddr> ptrAddr;
    ta->firstAddr();
    while ( ptrAddr = ta->getAddr() ) {
        if (*ptrAddr->get()==*clntAddr)
            break;
    }

    // address already exists
    if (!ptrAddr) {
        Log(Warning) << "Temp. address " << *clntAddr << " not assigned, cannot delete." << LogEnd;
        return false;
    }

    ta->delAddr(clntAddr);
    if (!quiet)
        Log(Debug) << "Deleted temp. address " << *clntAddr << " from addrDB." << LogEnd;

    if (!ta->countAddr()) {
        if (!quiet)
            Log(Debug) << "Deleted TA (IAID=" << iaid << ") from addrDB." << LogEnd;
        ptrClient->delTA(iaid);
    }

    if (!ptrClient->countIA() && !ptrClient->countTA() && !ptrClient->countPD()) {
        if (!quiet)
            Log(Debug) << "Deleted client (DUID=" << clntDuid->getPlain()
                       << ") from addrDB." << LogEnd;
        this->delClient(clntDuid);
    }

    return true;
}

bool TSrvAddrMgr::delPrefix(SPtr<TDUID> clntDuid, unsigned long IAID, SPtr<TIPv6Addr> prefix, bool quiet)
{
    bool result = TAddrMgr::delPrefix(clntDuid, IAID, prefix, quiet);
    if (result)
        addCachedEntry(clntDuid, prefix, IATYPE_PD);
    return result;
}

/// @brief returns how many leases does this client have?
///
/// @param duid client's DUID
///
/// @return number of leases (addresses and/or prefixes)
unsigned long TSrvAddrMgr::getLeaseCount(SPtr<TDUID> duid) {
    SPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() ) {
        if ( (*ptrClient->getDUID()) == (*duid))
            break;
    }
    // Have we found this client?
    if (!ptrClient) {
        return 0;
    }

    unsigned long count = 0;

    // count each of client's IAs
    SPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();
    while ( ptrIA = ptrClient->getIA() ) {
        count += ptrIA->countAddr();
    }

    // count each of client's TA
    ptrClient->firstTA();
    while (ptrIA = ptrClient->getTA() ) {
        count += ptrIA->countAddr();
    }

    // count each of client's PD
    ptrClient->firstPD();
    while (ptrIA = ptrClient->getPD() ) {
        count += ptrIA->countPrefix();
    }

    return count;
}

bool TSrvAddrMgr::addrIsFree(SPtr<TIPv6Addr> addr)
{
    // for each client...
    SPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() )
    {

        // look at each client's IAs
        SPtr <TAddrIA> ptrIA;
        ptrClient->firstIA();
        while ( ptrIA = ptrClient->getIA() )
        {
            SPtr<TAddrAddr> ptrAddr;
            if (ptrAddr = ptrIA->getAddr(addr) )
                return false;
        }
    }
    return true;
}

/**
 * Verifies if addr is unused
 *
 * @param addr
 *
 * @return
 */
bool TSrvAddrMgr::taAddrIsFree(SPtr<TIPv6Addr> addr)
{
    // for each client...
    SPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() ) {
        // look at each client's TAs
        SPtr <TAddrIA> ta;
        ptrClient->firstTA();
        while ( ta = ptrClient->getTA() )
        {
            if (ta->getAddr(addr) )
                return false;
        }
    }
    return true;
}

void TSrvAddrMgr::getAddrsCount(SPtr< List(TSrvCfgAddrClass) > classes,
     long *clntCnt, long *addrCnt, SPtr<TDUID> duid, int iface)
{
    memset(clntCnt,0,sizeof(long)*classes->count());
    memset(addrCnt,0,sizeof(long)*classes->count());
    int classNr=0;

    SPtr<TAddrClient> ptrClient;
    firstClient();
    while(ptrClient=getClient())
    {
        bool thisClient=(*(ptrClient->getDUID())==*duid);
        ptrClient->firstIA();
        SPtr<TAddrIA> ptrIA;
        while(ptrIA=ptrClient->getIA())
        {
            SPtr<TAddrAddr> ptrAddr;
            ptrIA->firstAddr();
            while(ptrAddr=ptrIA->getAddr())
            {
                if(ptrIA->getIfindex() == iface)
                {
                    SPtr<TSrvCfgAddrClass> ptrClass;
                    classes->first();
                    classNr=0;
                    while(ptrClass=classes->get())
                    {
                        if(ptrClass->addrInPool(ptrAddr->get()))
                        {
                            if(thisClient)
                                clntCnt[classNr]++;
                            addrCnt[classNr]++;
                        }
                        classNr++;
                    }
                }
            }
        }
    }
}

SPtr<TIPv6Addr> TSrvAddrMgr::getFirstAddr(SPtr<TDUID> clntDuid)
{
    SPtr<TAddrClient> ptrAddrClient = this->getClient(clntDuid);
    if (!ptrAddrClient) {
        Log(Warning) << "Unable to find client in the addrDB." << LogEnd;
        return SPtr<TIPv6Addr>();
    }
    ptrAddrClient->firstIA();
    SPtr<TAddrIA> ptrAddrIA = ptrAddrClient->getIA();
    if (!ptrAddrIA) {
        Log(Warning) << "Client does not have any addresses assigned." << LogEnd;
        return SPtr<TIPv6Addr>();
    }
    ptrAddrIA->firstAddr();
    SPtr<TAddrAddr> addr = ptrAddrIA->getAddr();
    if (!addr) {
        return SPtr<TIPv6Addr>();
    }
    return addr->get();
}



/* ******************************************************************************** */
/* *** ADDRESS CACHE ************************************************************** */
/* ******************************************************************************** */

/// @brief  remove outdated addresses
///
/// @param addrLst
/// @param tempAddrLst
/// @param prefixLst
///
void TSrvAddrMgr::doDuties(std::vector<TExpiredInfo>& addrLst,
                           std::vector<TExpiredInfo>& tempAddrLst,
                           std::vector<TExpiredInfo>& prefixLst)
{
    SPtr<TAddrClient> ptrClient;
    SPtr<TAddrIA>     ptrIA;
    SPtr<TAddrAddr>   ptrAddr;

    // for each client...
    this->firstClient();
    while (ptrClient = this->getClient() )
    {
        // ... which has outdated addresses
        if (ptrClient->getValidTimeout())
            continue;

        // check for expired addresses
        ptrClient->firstIA();
        while ( ptrIA = ptrClient->getIA() )
        {
            // has this IA outdated addressess?
            if ( ptrIA->getValidTimeout() )
                continue;

            ptrIA->firstAddr();
            while ( ptrAddr = ptrIA->getAddr() )
            {
                if (ptrAddr->getValidTimeout())
                    continue;

                TExpiredInfo expire;
                expire.client = ptrClient;
                expire.ia = ptrIA;
                expire.addr = ptrAddr->get();
                addrLst.push_back(expire);
                // delClntAddr(ptrClient->getDUID(), ptrIA->getIAID(),  ptrAddr->get(), false);
            }
        }

        ptrClient->firstTA();
        while (ptrIA = ptrClient->getTA()) {
            if (ptrIA->getValidTimeout())
                continue;
            ptrIA->firstAddr();
            while ( ptrAddr = ptrIA->getAddr() )
            {
                if (ptrAddr->getValidTimeout())
                    continue;

                TExpiredInfo expire;
                expire.client = ptrClient;
                expire.ia = ptrIA;
                expire.addr = ptrAddr->get();
                tempAddrLst.push_back(expire);
                // delTAAddr(ptrClient->getDUID(), ptrIA->getIAID(), ptrAddr->get());
            }
        }

        SPtr<TAddrIA> pd;
        SPtr<TAddrPrefix> prefix;
        ptrClient->firstPD();
        while (pd = ptrClient->getPD()) {
            if (pd->getValidTimeout())
                continue;

            pd->firstPrefix();
            while (prefix = pd->getPrefix()) {
                if (prefix->getValidTimeout())
                    continue;

                TExpiredInfo expire;
                expire.client = ptrClient;
                expire.ia = pd;
                expire.addr = prefix->get();
                expire.prefixLen = prefix->getLength();
                prefixLst.push_back(expire);
                // delPrefix(ptrClient->getDUID(), pd->getIAID(), prefix->get(), false);
            } // while (prefix)
        } // while (pd)
    } // while (client)
}

/// @brief Checks if address is still supported in current configuration (used in loadDB)
///
/// @param addr checked address
///
/// @return true, if supported
bool TSrvAddrMgr::verifyAddr(SPtr<TIPv6Addr> addr) {
    if (SrvCfgMgr().addrReserved(addr)) {
        return true;
    }

    SrvCfgMgr().firstIface();
    while (SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIface()) {
        if (SrvCfgMgr().getClassByAddr(iface->getID(), addr)) {
            return true;
        }
    }
    return false;
}

/// @brief Checks if prefix is still supported in current configuration (used in loadDB)
///
/// @param prefix checked prefix
///
/// @return true, if prefix is supported
bool TSrvAddrMgr::verifyPrefix(SPtr<TIPv6Addr> prefix) {
    if (SrvCfgMgr().prefixReserved(prefix)) {
        return true;
    }

    SrvCfgMgr().firstIface();
    while (SPtr<TSrvCfgIface> iface = SrvCfgMgr().getIface()) {
        if (SrvCfgMgr().getClassByPrefix(iface->getID(), prefix)) {
            return true;
        }
    }
    return false;
}

/**
 * returns address or prefix cached for this client.
 *
 * @param clntDuid
 * @param type type of entry looked for (TYPE_IA for address or TYPE_PD for prefix)
 *
 * @return cached address or prefix. 0 if cached address is not found.
 */
SPtr<TIPv6Addr> TSrvAddrMgr::getCachedEntry(SPtr<TDUID> clntDuid, TIAType type) {
    if (!this->CacheMaxSize)
        return SPtr<TIPv6Addr>();
    SPtr<TSrvCacheEntry> entry;
    this->Cache.first();

    while (entry = this->Cache.get()) {
            if (!entry->Duid)
                continue; // something is wrong. VERY wrong. But shut up and continue.
            if ((entry->type==type) && (*entry->Duid == *clntDuid) ) {
                Log(Debug) << "Cache: Cached " << (type==IATYPE_IA?"address":"prefix")
                           << " for client (DUID=" << clntDuid->getPlain() << ") found: "
                           << entry->Addr->getPlain() << LogEnd;
                return entry->Addr;
            }
    }

    Log(Debug) << "Cache: There are no cached " << (type==IATYPE_IA?"address":"prefix")
               << " address entries for client (DUID=" << clntDuid->getPlain() << ")." << LogEnd;
    return SPtr<TIPv6Addr>();
}

/**
 * this function deletes cached address or prefix entry
 *
 * @param addr
 * @param type type of entry looked for (TYPE_IA for address or TYPE_PD for prefix)
 *
 * @return
 */
bool TSrvAddrMgr::delCachedEntry(SPtr<TIPv6Addr> addr, TIAType type) {
    if (!this->CacheMaxSize)
            return false;

    this->Cache.first();
    SPtr<TSrvCacheEntry> entry;
    while (entry = this->Cache.get()) {
            if (!entry->Addr)
                continue; // something is wrong. VERY wrong. But shut up and continue.
            if ( (entry->type==type) && (*(entry->Addr) == *addr) ) {
                this->Cache.del();
                this->Cache.first();
                Log(Debug) << "Cache: " << (type==IATYPE_IA?"Address ":"Prefix ")
                           << *addr << " was deleted." << LogEnd;
                return true;
            }
    }
    Log(Debug) << "Cache: Attempt to delete " << *addr << " failed." << LogEnd;
    return false;
}

 /**
 * this function deletes cache entry
 *
 * @param clntDuid
 * @param type type of entry looked for (TYPE_IA for address or TYPE_PD for prefix)
 *
 * @return
 */
bool TSrvAddrMgr::delCachedEntry(SPtr<TDUID> clntDuid, TIAType type) {
    if (!this->CacheMaxSize)
        return false;

    this->Cache.first();
    SPtr<TSrvCacheEntry> entry;
    while (entry = this->Cache.get()) {
        if (!entry->Duid)
            continue; // something is wrong. VERY wrong. But shut up and continue.
        if ( (entry->type==type) && (*(entry->Duid) == *clntDuid) ) {
            this->Cache.del();
            this->Cache.first();
            Log(Debug) << "Cache: Entry for client (DUID=" << clntDuid->getPlain() << ") was deleted." << LogEnd;
            return true;
        }
    }
    // delete attempt is done on multiple occasions as a safety precausion, so don't warn if it is missing
    // Log(Debug) << "Cache: Attempt to delete entry for client (DUID=" << clntDuid->getPlain() << ") failed." << LogEnd;
    return false;
}

/**
 * this function adds an address or prefix to a cache. If there is entry for this client, updates it
 *
 * @param clntDuid
 * @param cachedAddr
 * @param type type of entry looked for (TYPE_IA for address or TYPE_PD for prefix)
 */
void TSrvAddrMgr::addCachedEntry(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> cachedAddr, TIAType type) {

    // if cache is disabled (size set to 0)
    if (!this->CacheMaxSize)
        return;

    // if this address is reserved, don't add it to cache)
    if ( (type == IATYPE_IA) && (SrvCfgMgr().addrReserved(cachedAddr)))
        return;

    // if this prefix is reserved, don't add it to cache)
    if ( (type == IATYPE_PD) && (SrvCfgMgr().prefixReserved(cachedAddr)))
        return;

    SPtr<TSrvCacheEntry> entry;

    // is there an entry for this client, delete it. New entry will be added at the end
    this->delCachedEntry(clntDuid, type);

    entry = new TSrvCacheEntry();
    entry->type = type;
    entry->Duid = clntDuid;
    entry->Addr = cachedAddr;
    Log(Debug) << "Cache: " << (type==IATYPE_IA?"Address ":"Prefix ") << cachedAddr->getPlain()
               << " added for client (DUID=" << clntDuid->getPlain() << "). " << LogEnd;
    this->Cache.append(entry);
    this->checkCacheSize();
}

void TSrvAddrMgr::setCacheSize(int bytes) {
    int entrySize = sizeof(TSrvCacheEntry) + sizeof(TIPv6Addr) + sizeof(TDUID);
    this->CacheMaxSize = bytes/entrySize;
    Log(Debug) << "Cache: size set to " << bytes << " bytes, 1 cache entry size is " << entrySize
                   << " bytes, so maximum " << this->CacheMaxSize << " address-client pair(s) may be cached." << LogEnd;
    this->checkCacheSize();
}

/**
 * this function checks if the cache size was not exceeded. If that is so, oldest entries are removed
 *
 */
void TSrvAddrMgr::checkCacheSize() {
    if (this->Cache.count() <= this->CacheMaxSize)
        return;

    // there are too many cached elements, delete some
    while (this->Cache.count() > this->CacheMaxSize) {
        this->Cache.delFirst();
    }
}

void TSrvAddrMgr::print(std::ostream & out) {
    out << "  <cache size=\"" << this->Cache.count() << "\"/>" << endl;
}

void TSrvAddrMgr::dump() {

    // Do not write anything to disk if there is performance mode enabled
    if (SrvCfgMgr().getPerformanceMode())
        return;

    TAddrMgr::dump(); // perform normal dump of the AddrMgr
    cacheDump();
}

/**
 * dumps address cache into a file specified by SRVCACHE_FILE
 *
 */
void TSrvAddrMgr::cacheDump() {
    std::ofstream f;
    f.open(SRVCACHE_FILE);
    if (!f.is_open()) {
        Log(Error) << "Cache: File " << SRVCACHE_FILE << " creation failed." << LogEnd;
        return;
    }
    f << "<cache size=\"" << this->Cache.count() << "\">" << endl;
    SPtr<TSrvCacheEntry> x;
    this->Cache.first();
    while (x=this->Cache.get()) {
        f << "  <entry type=\"";
        switch (x->type) {
        case IATYPE_IA:
            f << "addr";
            break;
        case IATYPE_PD:
            f << "prefix";
            break;
        default:
            Log(Error) << "Invalid cache type entry. Skipping." << LogEnd;
            continue;
        }

        f << "\" duid=\"";
        if (x->Duid)
            f << x->Duid->getPlain();
        f << "\">";
        if (x->Addr)
            f << x->Addr->getPlain();
        f << "</entry>" << endl;
    }
    f << "</cache>" << endl;
    f.close();
}

/**
 * dumps address cache into a file specified by SRVCACHE_FILE
 *
 */
void TSrvAddrMgr::cacheRead() {

    this->Cache.clear();
    bool started = false;
    bool ended = false;
    bool parsed = false;
    int lineno = 0;
    size_t entries = 0;
    std::ifstream f;
    string s;
    TIAType type;
    f.open(SRVCACHE_FILE);
    if (!f.is_open()) {
        Log(Warning) << "Cache: Unable to open cache file " << SRVCACHE_FILE << "." << LogEnd;
        return;
    }

    parsed = false;
    while (!f.eof()) {
        getline(f,s);
        string::size_type pos=0;
        if ( ((pos = s.find("<cache")) != string::npos) ) {
            // parse beginning

            started = true;
            if ( (pos = s.find("size=\"")) != string::npos ) {
                s = s.substr(pos+6);
                entries = atoi(s.c_str());
                Log(Debug) << "Cache:" << SRVCACHE_FILE << " file: parsing started, expecting "
                           << entries << " entries." << LogEnd;
            } else {
                Log(Debug) << "Cache:" << SRVCACHE_FILE << " file:unable to find entries count. "
                           << "size=\"...\" missing in line " << lineno << "." << LogEnd;
                return;
            }
        }

        if (s.find("<entry")!=string::npos) {
            if (!started) {
                Log(Error) << "Cache:" << SRVCACHE_FILE << " file: opening tag <cache> missing."
                           << LogEnd;
                return;
            }

            type = IATYPE_IA; // assume that entry is for address (to maintain backward compatibility)

            if ( (pos=s.find("type=\""))!=string::npos) {
                s = s.substr(pos+6);
                string typeStr = s.substr(0, s.find("\""));
                if (typeStr =="addr")
                    type = IATYPE_IA;
                else if (typeStr =="prefix")
                    type = IATYPE_PD;
                else {
                    Log(Error) << "Invalid entry type in line " << lineno << " in " << SRVCACHE_FILE << LogEnd;
                    continue;
                }
            }

            if ( (pos=s.find("duid=\""))!=string::npos) {
                s = s.substr(pos+6);
                string duid = s.substr(0, s.find("\""));
                s = s.substr(s.find("\"")+2);
                string addr = s.substr(0, s.find("<"));

                SPtr<TIPv6Addr> tmp2 = new TIPv6Addr(addr.c_str(), true);
                SPtr<TDUID>     tmp1 = new TDUID(duid.c_str());
                addCachedEntry(tmp1, tmp2, type);
            } else {
                Log(Error) << "Cache: " << SRVCACHE_FILE << " file: missing duid=\"...\" in line " << lineno
                           << "." << LogEnd;
                return;
            }
        }

        if (s.find("</cache>")!=string::npos) {
            if (!started) {
                Log(Error) << "Cache: Reading file " << SRVCACHE_FILE << " failed: closing tag </cache> found at line "
                           << lineno << ", but opening tag is missing." << LogEnd;
                f.close();
                return;
            }
            parsed = true;
            ended = true;
        }
    }
    f.close();

    if (this->Cache.count() != entries) {
        Log(Debug) << "Cache: " << SRVCACHE_FILE << " file: " << entries << " entries expected, but "
                   << this->Cache.count() << " found." << LogEnd;
    }
    if (!parsed) {
        Log(Info) << "Did not find any useful information in " << SRVCACHE_FILE << LogEnd;
    }
    if (!ended) {
        Log(Warning) << SRVCACHE_FILE << " seems truncated." << LogEnd;
    }
}

void TSrvAddrMgr::instanceCreate(const std::string& xmlFile, bool loadDB)
{
    if (Instance) {
        Log(Crit) << "SrvAddrMgr already exists! Application error" << LogEnd;
        return;
    }
    Instance = new TSrvAddrMgr(xmlFile, loadDB);
}

TSrvAddrMgr & TSrvAddrMgr::instance()
{
    if (!Instance) {
        Log(Crit) << "SrvAddrMgr not created yet. Application error. Emergency shutdown." << LogEnd;
        exit(EXIT_FAILURE);
    }
    return *Instance;
}
