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

class TSrvCfgMgr : public TCfgMgr
{
public:  
    TSrvCfgMgr(
        SmartPtr<TSrvIfaceMgr> ifaceMgr, 
        string cfgFile,
        string oldCfgFile);
    
    //Interfaces acccess methods
    void firstIface();
    SmartPtr<TSrvCfgIface> getIface();
    long countIface();
    void addIface(SmartPtr<TSrvCfgIface> iface);
    int countIfaces();
    SmartPtr<TSrvCfgIface> getIface(int ifaceid);
    //Other access methods
    string getWorkDir();
    
    //Address assignment connected methods
    long getMaxAddrsPerClient(
        SmartPtr<TDUID> duid, 
        SmartPtr<TIPv6Addr> clntAddr, 
        int iface);
    long getFreeAddrs(
        SmartPtr<TDUID> duid, 
        SmartPtr<TIPv6Addr> clntAddr, 
        int iface);
    bool addrIsSupported(
        SmartPtr<TDUID> duid, 
        SmartPtr<TIPv6Addr> clntAddr, 
        SmartPtr<TIPv6Addr> addr);
    SmartPtr<TSrvCfgAddrClass> getClassByAddr(
        int iface, 
        SmartPtr<TIPv6Addr> addr);
    SmartPtr<TIPv6Addr> getRandomAddr(
        SmartPtr<TDUID> duid, 
        SmartPtr<TIPv6Addr> clntAddr, 
        long T1,long T2,long Pref,long Valid);
    
    SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > getClassesForClient(
        SmartPtr<TDUID> duid, 
        SmartPtr<TIPv6Addr> clntAddr, 
        int iface, bool rapid=false);
    
    bool isClntSupported(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);

    virtual ~TSrvCfgMgr();
    
    bool isDone();

private:    
    bool IsDone;
    bool checkConfigConsistency();
    string WorkDir;
    string LogName;
    string LogLevel;
    List(TSrvCfgIface) SrvCfgIfaceLst;
    SmartPtr<TSrvIfaceMgr> IfaceMgr;
};

#endif /* SRVCONFMGR_H */
