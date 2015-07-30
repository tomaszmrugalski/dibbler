/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 * changes: Grzegorz Pluto   <g.pluto(at)u-r-b-a-n(dot)pl>
 *
 * released under GNU GPL v2 licence
 */

class TAddrMgr;
#ifndef ADDRMGR_H
#define ADDRMGR_H

#include <string>
#include <map>
#include "SmartPtr.h"
#include "Container.h"
#include "AddrClient.h"
#include "AddrIA.h"

///
/// @brief Address Manager that holds address and prefix information.
///
/// This class holds information about assigned leases: addresses
/// and prefixes with additional associated information: list of
/// clients, listf of IAs, list of addresses, peer addresses,
/// associated FQDN names, DNS addresses that performed DNS Update,
/// t1,t2,prefered,valid lifetimes and similar data.
///
/// TAddrMgr is used by both server and client.
///
/// TAddrMgr has a container for TAddrClient - a list of clients.
/// Each TAddrClient contains list of Identity Associations (IAs),
/// represented by TAddrIA. Each TAddrIA contains list of addresses
/// (TAddrAddr) or prefixes (TAddrPrefix).
///
class TAddrMgr
{
  public:

    /// holds network interface name to ifindex mapping
    typedef std::map<std::string, int> NameToIndexMapping;

    /// holds network interface ifindex to name mapping
    typedef std::map<int, std::string> IndexToNameMapping;

    friend std::ostream & operator<<(std::ostream & strum,TAddrMgr &x);
    TAddrMgr(const std::string& addrdb, bool loadfile = false);
    virtual ~TAddrMgr();

    bool updateInterfacesInfo(const NameToIndexMapping& nameToIndex,
                              const IndexToNameMapping& indexToName);

    bool updateInterfacesInfoIA(SPtr<TAddrIA> ia,
                                const NameToIndexMapping& nameToIndex,
                                const IndexToNameMapping& indexToName);

    //--- Client container ---
    void addClient(SPtr<TAddrClient> x);
    void firstClient();
    SPtr<TAddrClient> getClient();
    SPtr<TAddrClient> getClient(SPtr<TDUID> duid);
    SPtr<TAddrClient> getClient(uint32_t SPI);
    SPtr<TAddrClient> getClient(SPtr<TIPv6Addr> leasedAddr);
    int countClient();
    bool delClient(SPtr<TDUID> duid);

    // checks if address is conformant to current configuration (used in loadDB())
    virtual bool verifyAddr(SPtr<TIPv6Addr> addr) { return true; }
    virtual bool verifyPrefix(SPtr<TIPv6Addr> addr) { return true; }

    // --- prefix related ---
    virtual bool addPrefix(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
                           const std::string& ifname,
                           int ifindex, unsigned long IAID, unsigned long T1, unsigned long T2,
                           SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
                           int length, bool quiet);
    virtual bool updatePrefix(SPtr<TDUID> duid , SPtr<TIPv6Addr> addr,
                              const std::string& ifname,
                              int ifindex, unsigned long IAID, unsigned long T1,
                              unsigned long T2, SPtr<TIPv6Addr> prefix, unsigned long pref,
                              unsigned long valid, int length, bool quiet);

    virtual bool delPrefix(SPtr<TDUID> clntDuid, unsigned long IAID,
                           SPtr<TIPv6Addr> prefix, bool quiet);
    bool prefixIsFree(SPtr<TIPv6Addr> prefix);

    //--- Time related methods ---
    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();

    // --- backup/restore ---
    void dbLoad(const char * xmlFile);
    virtual void dump();
    bool isDone();

#ifdef MOD_LIBXML2
    // database loading methods that use libxml2
    xmlDocPtr xmlLoad(const char * filename);
    SPtr<TAddrAddr> parseAddrAddr(xmlDocPtr doc, xmlNodePtr xmlAddr, int depth);
    SPtr<TAddrIA> libxml_parseAddrIA(xmlDocPtr doc, xmlNodePtr xmlIA, int depth);
    SPtr<TAddrClient> parseAddrClient(xmlDocPtr doc, xmlNodePtr xmlClient, int depth);
    void parseAddrMgr(xmlDocPtr doc,int depth);
#else
    // database loading methods that use internal loading routines
    bool xmlLoadBuiltIn(const char * xmlFile);
    SPtr<TAddrClient> parseAddrClient(const char * xmlFile, FILE *f);
    SPtr<TAddrIA> parseAddrIA(const char * xmlFile, FILE * f, int t1,int t2,
                              int iaid, const std::string& ifname, int ifindex,
                              SPtr<TIPv6Addr> unicast = SPtr<TIPv6Addr>());
    SPtr<TAddrIA> parseAddrPD(const char * xmlFile, FILE * f, int t1,int t2,
                              int iaid, const std::string& ifname, int ifindex,
                              SPtr<TIPv6Addr> unicast = SPtr<TIPv6Addr>());
    SPtr<TAddrAddr> parseAddrAddr(const char * xmlFile, char * buf,bool pd);
    SPtr<TAddrPrefix> parseAddrPrefix(const char * xmlFile, char * buf,bool pd);
    SPtr<TAddrIA> parseAddrTA(const char * xmlFile, FILE *f);
#endif

    uint64_t getNextReplayDetectionValue();

protected:
    virtual void print(std::ostream & out) = 0;
    bool addPrefix(SPtr<TAddrClient> client, SPtr<TDUID> duid , SPtr<TIPv6Addr> clntAddr,
                   const std::string& ifname, int ifindex, unsigned long IAID,
                   unsigned long T1, unsigned long T2, SPtr<TIPv6Addr> prefix,
                   unsigned long pref, unsigned long valid, int length, bool quiet);
    bool updatePrefix(SPtr<TAddrClient> client, SPtr<TDUID> duid , SPtr<TIPv6Addr> clntAddr,
                      int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
                      SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
                      int length, bool quiet);

    bool IsDone;
    List(TAddrClient) ClntsLst;
    std::string XmlFile;

    /// should the client without any IA, TA or PDs be deleted? (srv = yes, client = no)
    bool DeleteEmptyClient;

    uint64_t ReplayDetectionValue_;
};

#endif
