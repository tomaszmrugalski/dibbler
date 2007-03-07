/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelCfgMgr.h,v 1.5 2007-03-07 02:37:11 thomson Exp $
 *
 */

class TRelCfgMgr;
#ifndef RELCFGMGR_H
#define RELCFGMGR_H

#include "RelCommon.h"
#include "RelCfgIface.h"

#include "CfgMgr.h"
#include "DHCPConst.h"

class TRelCfgMgr : public TCfgMgr
{
public:  
    friend ostream & operator<<(ostream &strum, TRelCfgMgr &x);
    TRelCfgMgr(TCtx ctx, string cfgFile, string xmlFile);
    virtual ~TRelCfgMgr();

    bool parseConfigFile(string cfgFile);

    //Interfaces acccess methods
    void firstIface();
    SmartPtr<TRelCfgIface> getIface();
    SmartPtr<TRelCfgIface> getIfaceByID(int iface);
    SmartPtr<TRelCfgIface> getIfaceByInterfaceID(int iface);
    long countIface();
    void addIface(SmartPtr<TRelCfgIface> iface);

    void dump();

    bool isDone();

    bool setupGlobalOpts(SmartPtr<TRelParsGlobalOpt> opt);

    // configuration parameters
    string getWorkdir();

    bool guessMode();

private:    
    TCtx Ctx;
    string XmlFile;

    static int NextRelayID;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SmartPtr<TRelCfgIface> ptrIface);
    List(TRelCfgIface) IfaceLst;

    bool matchParsedSystemInterfaces(List(TRelCfgIface) * lst);

    // global options
    string Workdir;
    bool GuessMode;
};

#endif /* RELCONFMGR_H */
