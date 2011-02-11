/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *          Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrvCfgMgr;

#ifndef SRVCONFMGR_H
#define SRVCONFMGR_H
#include "SmartPtr.h"
#include "SrvCfgIface.h"
#include "SrvIfaceMgr.h"
#include "CfgMgr.h"
#include "DHCPConst.h"
#include "Container.h"
#include "DUID.h"
#include "KeyList.h"
#include "SrvCfgClientClass.h"


#define SrvCfgMgr() (TSrvCfgMgr::instance())

class SrvParser;

class TSrvCfgMgr : public TCfgMgr
{
public:
    friend ostream & operator<<(ostream &strum, TSrvCfgMgr &x);

    static void instanceCreate(const std::string cfgFile, const std::string xmlDumpFile);
    static TSrvCfgMgr &instance();

    bool parseConfigFile(string cfgFile);

    //Interfaces acccess methods
    void firstIface();
    SPtr<TSrvCfgIface> getIface();
    SPtr<TSrvCfgIface> getIfaceByID(int iface);
    long countIface();
    void addIface(SPtr<TSrvCfgIface> iface);
    void makeInactiveIface(int ifindex, bool inactive);
    int inactiveIfacesCnt();
    SPtr<TSrvCfgIface> checkInactiveIfaces();

    void dump();

    bool setupRelay(SPtr<TSrvCfgIface> cfgIface);

    //Address assignment connected methods
    void setCounters();
    long countAvailAddrs(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr, int iface);
    SPtr<TSrvCfgAddrClass> getClassByAddr(int iface, SPtr<TIPv6Addr> addr);
    SPtr<TSrvCfgPD> getClassByPrefix(int iface, SPtr<TIPv6Addr> prefix);
    SPtr<TIPv6Addr> getRandomAddr(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, int iface);
    bool isClntSupported(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, int iface);
    bool isClntSupported(SPtr<TDUID> duid, SPtr<TIPv6Addr> clntAddr, int iface, SPtr<TSrvMsg> msg);

    // prefix-related
    bool incrPrefixCount(int iface, SPtr<TIPv6Addr> prefix);
    bool decrPrefixCount(int iface, SPtr<TIPv6Addr> prefix);

    // class' usage management
    void delClntAddr(int iface, SPtr<TIPv6Addr> addr);
    void addClntAddr(int iface, SPtr<TIPv6Addr> addr);

    bool isIAAddrSupported(int iface, SPtr<TIPv6Addr> addr);
    bool isTAAddrSupported(int iface, SPtr<TIPv6Addr> addr);

    void addTAAddr(int iface);
    void delTAAddr(int iface);

    bool isDone();
    virtual ~TSrvCfgMgr();
    bool setGlobalOptions(SPtr<TSrvParsGlobalOpt> opt);

    // configuration parameters
    string getWorkdir();
    bool stateless();
    bool inactiveMode();
    bool guessMode();
    ESrvIfaceIdOrder getInterfaceIDOrder();
    int getCacheSize();
    bool reconfigureSupport();

    void fqdnDdnsAddress(SPtr<TIPv6Addr> ddnsAddress);
    SPtr<TIPv6Addr> fqdnDdnsAddress();

    // Bulk-LeaseQuery
    void bulkLQAccept(bool enabled);
    void bulkLQTcpPort(unsigned short portNumber);
    void bulkLQMaxConns(unsigned int maxConnections);
    void bulkLQTimeout(unsigned int timeout);

    //Authentication
#ifndef MOD_DISABLE_AUTH
    SPtr<KeyList> AuthKeys;
    unsigned int getAuthLifetime();
    unsigned int getAuthKeyGenNonceLen();
    List(DigestTypes) getDigestLst();
    enum DigestTypes getDigest();
#endif

    void setDefaults();

    // Client List check
    void InClientClass(SPtr<TSrvMsg> msg);
private:
    TSrvCfgMgr(string cfgFile, string xmlFile);
    static TSrvCfgMgr * Instance;
    static int NextRelayID;
    string XmlFile;
    bool reconfigure;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SPtr<TSrvCfgIface> ptrIface);
    bool validateClass(SPtr<TSrvCfgIface> ptrIface, SPtr<TSrvCfgAddrClass> ptrClass);
    List(TSrvCfgIface) SrvCfgIfaceLst;
    List(TSrvCfgIface) InactiveLst;
    List(TSrvCfgClientClass) ClientClassLst;
    bool matchParsedSystemInterfaces(SrvParser *parser);

    // global options
    bool Stateless;
    bool InactiveMode;
    bool GuessMode;
    int  CacheSize;
    ESrvIfaceIdOrder InterfaceIDOrder;

#ifndef MOD_DISABLE_AUTH
    unsigned int AuthLifetime;
    unsigned int AuthKeyGenNonceLen;
    List(DigestTypes) DigestLst;
#endif

    // DDNS address
    SPtr<TIPv6Addr> FqdnDdnsAddress;

    // lease-query parameters
    bool BulkLQAccept;
    unsigned short BulkLQTcpPort;
    unsigned int BulkLQMaxConns;
    unsigned int BulkLQTimeout;
};

#endif /* SRVCONFMGR_H */
