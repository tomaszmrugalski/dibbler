/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef RELCFGMGR_H
#define RELCFGMGR_H

#include "RelCfgIface.h"

#include "CfgMgr.h"
#include "DHCPConst.h"
#include "OptVendorData.h"
#include "RelOptEcho.h"

#define RelCfgMgr() (TRelCfgMgr::instance())

class TRelCfgMgr : public TCfgMgr
{
public:  
    friend ostream & operator<<(std::ostream &strum, TRelCfgMgr &x);
    virtual ~TRelCfgMgr();

    static void instanceCreate(const std::string cfgFile, const std::string xmlFile);
    static TRelCfgMgr& instance();

    bool parseConfigFile(const std::string cfgFile);

    //Interfaces access methods
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

    SPtr<TOptVendorData> getRemoteID();
    SPtr<TRelOptEcho>     getEcho();

private:
    static TRelCfgMgr * Instance;
    TRelCfgMgr(const std::string cfgFile, const std::string xmlFile);

    std::string XmlFile;

    static int NextRelayID;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SPtr<TRelCfgIface> ptrIface);
    List(TRelCfgIface) IfaceLst;

    bool matchParsedSystemInterfaces(List(TRelCfgIface) * lst);

    // global options
    std::string Workdir;
    bool GuessMode;
    ERelIfaceIdOrder InterfaceIDOrder;

    SPtr<TOptVendorData> RemoteID;
    SPtr<TRelOptEcho> Echo;
};

#endif /* RELCONFMGR_H */
