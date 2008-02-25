/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof WNuk <keczi@poczta.onet.pl>                                                                         
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgMgr.h,v 1.20 2008-02-25 17:49:06 thomson Exp $
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
#include "KeyList.h"

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

    bool anonInfRequest();
    bool insistMode();
    bool inactiveMode();

    int inactiveIfacesCnt();
    SPtr<TClntCfgIface> checkInactiveIfaces();
    bool openSocket(SPtr<TClntCfgIface> iface);

    // Authorization
    uint32_t getAAASPI();
    List(DigestTypes) getAuthAcceptMethods();
    bool getAuthEnabled();
    SmartPtr<KeyList> AuthKeys;

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
    bool AnonInfRequest;
    bool InsistMode;
    bool InactiveMode;

    bool AuthEnabled;
    List(DigestTypes) AuthAcceptMethods;

    uint32_t AAASPI;
};

typedef bool HardcodedCfgFunc(TClntCfgMgr *cfgMgr, string params);

#endif
