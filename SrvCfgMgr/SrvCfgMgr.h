/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgMgr.h,v 1.8 2005-01-03 21:57:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2004/12/07 00:43:03  thomson
 * Server no longer support link local addresses (bug #38),
 * Server now supports stateless mode (bug #71)
 *
 * Revision 1.6  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.5  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.4  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
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

#define RELAY_MIN_IFINDEX 1024;

class TSrvCfgMgr : public TCfgMgr
{
public:  
    friend ostream & operator<<(ostream &strum, TSrvCfgMgr &x);
    TSrvCfgMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr, string cfgFile, string xmlFile);

    bool parseConfigFile(string cfgFile);

    //Interfaces acccess methods
    void firstIface();
    SmartPtr<TSrvCfgIface> getIface();
    SmartPtr<TSrvCfgIface> getIfaceByID(int iface);
    long countIface();
    void addIface(SmartPtr<TSrvCfgIface> iface);

    void dump();

    bool setupRelay(SmartPtr<TSrvCfgIface> cfgIface);
    
    //Address assignment connected methods
    long countAvailAddrs(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    SmartPtr<TSrvCfgAddrClass> getClassByAddr(int iface, SmartPtr<TIPv6Addr> addr);
    SmartPtr<TIPv6Addr> getRandomAddr(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);
    bool isClntSupported(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> clntAddr, int iface);

    // class' usage management
    void delClntAddr(int iface, SmartPtr<TIPv6Addr> addr);
    void addClntAddr(int iface, SmartPtr<TIPv6Addr> addr);

    bool isDone();
    virtual ~TSrvCfgMgr();
    bool setupGlobalOpts(SmartPtr<TSrvParsGlobalOpt> opt);

    // configuration parameters
    string getWorkdir();
    bool stateless();
    static int NextRelayID;

private:    
    string XmlFile;

    bool IsDone;
    bool validateConfig();
    bool validateIface(SmartPtr<TSrvCfgIface> ptrIface);
    bool validateClass(SmartPtr<TSrvCfgIface> ptrIface, SmartPtr<TSrvCfgAddrClass> ptrClass);
    string LogName;
    string LogLevel;
    List(TSrvCfgIface) SrvCfgIfaceLst;
    SmartPtr<TSrvIfaceMgr> IfaceMgr;
    bool matchParsedSystemInterfaces(SrvParser *parser);

    // global options
    string Workdir;
    bool Stateless;
};

#endif /* SRVCONFMGR_H */
