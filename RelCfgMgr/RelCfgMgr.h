/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelCfgMgr.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

class TRelCfgMgr;
#ifndef RELCFGMGR_H
#define RELCFGMGR_H

#include "RelCommon.h"
#include "RelCfgIface.h"

#include "CfgMgr.h"
#include "DHCPConst.h"

#define RELAY_MIN_IFINDEX 1024;

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
    long countIface();
    void addIface(SmartPtr<TRelCfgIface> iface);

    void dump();

    bool isDone();

    bool setupGlobalOpts(SmartPtr<TRelParsGlobalOpt> opt);

    // configuration parameters
    string getWorkdir();

private:    
    TCtx Ctx;
    string XmlFile;

    static int NextRelayID;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SmartPtr<TRelCfgIface> ptrIface);
    string LogName;
    string LogLevel;
    List(TRelCfgIface) IfaceLst;

    bool matchParsedSystemInterfaces(List(TRelCfgIface) * lst);

    // global options
    string Workdir;
};

#endif /* RELCONFMGR_H */
