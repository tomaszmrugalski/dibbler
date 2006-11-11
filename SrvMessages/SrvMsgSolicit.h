/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *  changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                        
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgSolicit.h,v 1.5 2006-11-11 06:56:27 thomson Exp $
 *
 */

class TSrvMsgSolicit;
#ifndef SRVMSGSOLICIT_H
#define SRVMSGSOLICIT_H
#include "SrvMsg.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvAddrMgr.h"

#include "SmartPtr.h"

class TSrvMsgSolicit : public TSrvMsg
{
 public:
    TSrvMsgSolicit(SmartPtr<TSrvIfaceMgr> ifaceMgr, 
		   SmartPtr<TSrvTransMgr> transMgr,
		   SmartPtr<TSrvCfgMgr>	  cfgMgr,
		   SmartPtr<TSrvAddrMgr>  addrMgr,
		   int iface, SmartPtr<TIPv6Addr> addr,
		   char* buf, int bufSzie);

    void doDuties();
    void sortAnswers();
    string getName();
    unsigned long getTimeout();
    bool check();
    ~TSrvMsgSolicit();

 private:
    void setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		    SmartPtr<TSrvTransMgr> TransMgr, 
		    SmartPtr<TSrvCfgMgr> CfgMgr,
		    SmartPtr<TSrvAddrMgr> AddrMgr,
		    int iface, char* addr, int msgType);
};
#endif /* SRVMSGSOLICIT_H*/
