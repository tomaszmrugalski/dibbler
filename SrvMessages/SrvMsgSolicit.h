/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *  changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                        
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgSolicit.h,v 1.4 2006-10-06 00:42:58 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005-01-08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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
	    int iface, 
	    char* addr, 
	    int msgType);
};
#endif /* SRVMSGSOLICIT_H*/
