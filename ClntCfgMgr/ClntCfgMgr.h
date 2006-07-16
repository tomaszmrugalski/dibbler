/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: ClntCfgMgr.h,v 1.11 2006-07-16 11:38:24 thomson Exp $
 */

class TClntCfgMgr;
#ifndef CLNTCFGMGR_H
#define CLNTCFGMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "ClntIfaceMgr.h"
#include "ClntCfgIface.h"
#include "CfgMgr.h"
#include "FlexLexer.h"
#include "ClntParser.h"

class TClntCfgMgr : public TCfgMgr
{
    friend ostream & operator<<(ostream &strum, TClntCfgMgr &x);
 public:
    TClntCfgMgr(SmartPtr<TClntIfaceMgr> IfaceMgr, 
		const string cfgFile,const string oldCfgFile);
    ~TClntCfgMgr();
    
    // --- Iface related ---
    SmartPtr<TClntCfgIA> getIA(long IAID);
    SmartPtr<TClntCfgIface> getIface();
    SmartPtr<TClntCfgIface> getIface(int id);
    void firstIface();
    void addIface(SmartPtr<TClntCfgIface> x);
    int countIfaces();
    void dump();
    
    bool getReconfigure();
    
    //IA related
    bool setIAState(int iface, int iaid, enum EState state);
    int countAddrForIA(long IAID);
    
    SmartPtr<TClntCfgIface> getIfaceByIAID(int iaid);
    bool isDone();

private:
    SmartPtr<TClntIfaceMgr> IfaceMgr;
    List(TClntCfgIface) ClntCfgIfaceLst;

    bool validateConfig();
    bool validateIface(SmartPtr<TClntCfgIface> iface);
    bool validateIA(SmartPtr<TClntCfgIface> ptrIface, SmartPtr<TClntCfgIA> ptrIA);
    bool validateAddr(SmartPtr<TClntCfgIface> ptrIface, 
		      SmartPtr<TClntCfgIA> ptrIA,
		      SmartPtr<TClntCfgAddr> ptrAddr);

    bool matchParsedSystemInterfaces(ClntParser *parser);
};

typedef bool HardcodedCfgFunc(TClntCfgMgr *cfgMgr, string params);

#endif
