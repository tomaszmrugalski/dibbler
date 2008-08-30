/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof WNuk <keczi@poczta.onet.pl>                                                                         
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: ClntCfgMgr.h,v 1.30 2008-08-30 21:41:10 thomson Exp $
 */

class TClntCfgMgr;
class TClntCfgIface;
class ClntParser;
#ifndef CLNTCFGMGR_H
#define CLNTCFGMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "ClntCfgIface.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIA.h"
#include "ClntCfgPD.h"
#include "CfgMgr.h"

#ifndef MOD_DISABLE_AUTH
#include "KeyList.h"
#endif

class TClntCfgMgr : public TCfgMgr
{
    friend ostream & operator<<(ostream &strum, TClntCfgMgr &x);
 public:
    TClntCfgMgr(SmartPtr<TClntIfaceMgr> IfaceMgr, const string cfgFile);
    ~TClntCfgMgr();
    
    // --- Iface related ---
    SmartPtr<TClntCfgIA> getIA(long IAID);
    SmartPtr<TClntCfgPD> getPD(long IAID);
    SmartPtr<TClntCfgIface> getIface();
    SmartPtr<TClntCfgIface> getIface(int id);
    void firstIface();
    void addIface(SmartPtr<TClntCfgIface> x);
    void makeInactiveIface(int ifindex, bool inactive);
    int countIfaces();
    void dump();
    
    bool getReconfigure();
    
    //IA related
    bool setIAState(int iface, int iaid, enum EState state);
    int countAddrForIA(long IAID);
    
    SmartPtr<TClntCfgIface> getIfaceByIAID(int iaid);
    bool isDone();

    DigestTypes getDigest();
    void setDigest(DigestTypes value);

    string getScriptsDir();
    bool getNotifyScripts();

    bool anonInfRequest();
    bool insistMode();
    bool inactiveMode();
    bool addInfRefreshTime();

    int inactiveIfacesCnt();
    SPtr<TClntCfgIface> checkInactiveIfaces();
    bool openSocket(SPtr<TClntCfgIface> iface);

#ifndef MOD_DISABLE_AUTH
    // Authorization
    uint32_t getAAASPI();
    List(DigestTypes) getAuthAcceptMethods();
    bool getAuthEnabled();
    SmartPtr<KeyList> AuthKeys;
#endif

    bool getFQDNFlagS();
    bool getMappingPrefix();

    bool useConfirm();

    int tunnelMode(); // returns vendor-id

private:
    bool setGlobalOptions(ClntParser * parser);
    bool validateConfig();
    bool validateIface(SmartPtr<TClntCfgIface> iface);
    bool validateIA(SmartPtr<TClntCfgIface> ptrIface, SmartPtr<TClntCfgIA> ptrIA);
    bool validateAddr(SmartPtr<TClntCfgIface> ptrIface, 
		      SmartPtr<TClntCfgIA> ptrIA,
		      SmartPtr<TClntCfgAddr> ptrAddr);
    bool parseConfigFile(string cfgFile);
    bool matchParsedSystemInterfaces(ClntParser *parser);

    SmartPtr<TClntIfaceMgr> IfaceMgr;
    List(TClntCfgIface) ClntCfgIfaceLst;
    List(TClntCfgIface) InactiveLst;
    DigestTypes Digest;
    string ScriptsDir;
    bool NotifyScripts;

    bool AnonInfRequest;
    bool InsistMode;
    bool InactiveMode;
    bool UseConfirm;

#ifndef MOD_DISABLE_AUTH
    bool AuthEnabled;
    List(DigestTypes) AuthAcceptMethods;
    uint32_t AAASPI;
#endif

    bool FQDNFlagS; // S bit in the FQDN option
    bool MappingPrefix;
    int  TunnelMode;
};

typedef bool HardcodedCfgFunc(TClntCfgMgr *cfgMgr, string params);

#endif
