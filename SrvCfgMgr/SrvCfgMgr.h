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
 * $Id: SrvCfgMgr.h,v 1.24 2008-10-12 20:07:31 thomson Exp $
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

#include "FlexLexer.h"
#include "SrvParser.h"
#include "SrvCfgClientClass.h"

class TSrvCfgMgr : public TCfgMgr
{
public:
    friend ostream & operator<<(ostream &strum, TSrvCfgMgr &x);
    TSrvCfgMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr, string cfgFile, string xmlFile);

    bool parseConfigFile(string cfgFile);

    //Interfaces acccess methods
    void firstIface();
    SmartPtr<TSrvCfgIface> getIface();
    SmartPtr<TSrvCfgIface> getIfaceByID(int iface);
    long countIface();
    void addIface(SmartPtr<TSrvCfgIface> iface);
    void makeInactiveIface(int ifindex, bool inactive);
    int inactiveIfacesCnt();
    SPtr<TSrvCfgIface> checkInactiveIfaces();

    void dump();

    bool setupRelay(SmartPtr<TSrvCfgIface> cfgIface);

    //Address assignment connected methods
    long countAvailAddrs(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    SmartPtr<TSrvCfgAddrClass> getClassByAddr(int iface, SmartPtr<TIPv6Addr> addr);
    SmartPtr<TSrvCfgPD> getClassByPrefix(int iface, SmartPtr<TIPv6Addr> prefix);
    SmartPtr<TIPv6Addr> getRandomAddr(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    bool isClntSupported(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    bool isClntSupported(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface, SmartPtr<TSrvMsg> msg);

    // prefix-related
    bool incrPrefixCount(int iface, SPtr<TIPv6Addr> prefix);
    bool decrPrefixCount(int iface, SPtr<TIPv6Addr> prefix);

    // class' usage management
    void delClntAddr(int iface, SmartPtr<TIPv6Addr> addr);
    void addClntAddr(int iface, SmartPtr<TIPv6Addr> addr);

    bool isIAAddrSupported(int iface, SmartPtr<TIPv6Addr> addr);
    bool isTAAddrSupported(int iface, SmartPtr<TIPv6Addr> addr);

    void addTAAddr(int iface);
    void delTAAddr(int iface);

    bool isDone();
    virtual ~TSrvCfgMgr();
    bool setGlobalOptions(SmartPtr<TSrvParsGlobalOpt> opt);

    // configuration parameters
    string getWorkdir();
    bool stateless();
    bool inactiveMode();
    bool guessMode();
    ESrvIfaceIdOrder getInterfaceIDOrder();
    int getCacheSize();

    //Authentication
#ifndef MOD_DISABLE_AUTH
    SmartPtr<KeyList> AuthKeys;
    unsigned int getAuthLifetime();
    unsigned int getAuthKeyGenNonceLen();
    List(DigestTypes) getDigestLst();
    enum DigestTypes getDigest();
#endif

    // Client List check
    void InClientClass(SmartPtr<TSrvMsg> msg);
private:
    static int NextRelayID;
    string XmlFile;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SmartPtr<TSrvCfgIface> ptrIface);
    bool validateClass(SmartPtr<TSrvCfgIface> ptrIface, SmartPtr<TSrvCfgAddrClass> ptrClass);
    List(TSrvCfgIface) SrvCfgIfaceLst;
    List(TSrvCfgIface) InactiveLst;
    List(TSrvCfgClientClass) ClientClassLst;
    SmartPtr<TSrvIfaceMgr> IfaceMgr;
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
};

#endif /* SRVCONFMGR_H */
