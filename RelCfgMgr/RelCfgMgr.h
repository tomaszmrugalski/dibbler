/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelCfgMgr.h,v 1.8 2008-08-29 00:07:32 thomson Exp $
 *
 */

class TRelCfgMgr;
#ifndef RELCFGMGR_H
#define RELCFGMGR_H

#include "RelCommon.h"
#include "RelCfgIface.h"

#include "CfgMgr.h"
#include "DHCPConst.h"
#include "RelOptRemoteID.h"
#include "RelOptEcho.h"

class TRelCfgMgr : public TCfgMgr
{
public:  
    friend ostream & operator<<(ostream &strum, TRelCfgMgr &x);
    TRelCfgMgr(TCtx ctx, string cfgFile, string xmlFile);
    virtual ~TRelCfgMgr();

    bool parseConfigFile(string cfgFile);

    //Interfaces acccess methods
    void firstIface();
    SPtr<TRelCfgIface> getIface();
    SPtr<TRelCfgIface> getIfaceByID(int iface);
    SPtr<TRelCfgIface> getIfaceByInterfaceID(int iface);
    long countIface();
    void addIface(SPtr<TRelCfgIface> iface);

    void dump();

    bool isDone();

    bool setupGlobalOpts(SPtr<TRelParsGlobalOpt> opt);

    // configuration parameters
    string getWorkdir();
    bool guessMode();
    ERelIfaceIdOrder getInterfaceIDOrder();

    SPtr<TRelOptRemoteID> getRemoteID();
    SPtr<TRelOptEcho>     getEcho();

private:    
    TCtx Ctx;
    string XmlFile;

    static int NextRelayID;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SPtr<TRelCfgIface> ptrIface);
    List(TRelCfgIface) IfaceLst;

    bool matchParsedSystemInterfaces(List(TRelCfgIface) * lst);

    // global options
    string Workdir;
    bool GuessMode;
    ERelIfaceIdOrder InterfaceIDOrder;

    SPtr<TRelOptRemoteID> RemoteID;
    SPtr<TRelOptEcho> Echo;
};

#endif /* RELCONFMGR_H */
