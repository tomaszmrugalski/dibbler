/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgMgr.h,v 1.5 2004-07-05 00:12:30 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 * Revision 1.3  2004/05/23 21:24:50  thomson
 * *** empty log message ***
 *
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
    TSrvCfgMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr, string cfgFile,string oldCfgFile);
    
    //Interfaces acccess methods
    void firstIface();
    SmartPtr<TSrvCfgIface> getIface();
    SmartPtr<TSrvCfgIface> getIfaceByID(int iface);
    long countIface();
    void addIface(SmartPtr<TSrvCfgIface> iface);

    void dump();
    
    //Address assignment connected methods
    long countAvailAddrs(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    SmartPtr<TSrvCfgAddrClass> getClassByAddr(int iface, SmartPtr<TIPv6Addr> addr);
    SmartPtr<TIPv6Addr> getRandomAddr(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    bool isClntSupported(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);

    // class' usage management
    void delClntAddr(int iface, SmartPtr<TIPv6Addr> addr);
    void addClntAddr(int iface, SmartPtr<TIPv6Addr> addr);

    string getWorkDir();
    bool isDone();
    virtual ~TSrvCfgMgr();

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
