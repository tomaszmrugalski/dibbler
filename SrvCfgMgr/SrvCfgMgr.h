/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgMgr.h,v 1.3 2004-05-23 21:24:50 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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

#include "FlexLexer.h"
#include "SrvParser.h"

class TSrvCfgMgr : public TCfgMgr
{
public:  
    friend ostream & operator<<(ostream &strum, TSrvCfgMgr &x);
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
    bool matchParsedSystemInterfaces(SrvParser *parser);
};

#endif /* SRVCONFMGR_H */
