/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgRequest.h,v 1.4 2006-08-21 21:33:21 thomson Exp $
 *                                                                           
 */

class TSrvMsgRequest;
#ifndef SRVMSGREQUEST_H
#define SRVMSGREQUEST_H

#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "IPv6Addr.h"

class TSrvMsgRequest : public TSrvMsg
{
  public:
    TSrvMsgRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr>   ConfMgr, 
		   SmartPtr<TSrvAddrMgr>  AddrMgr, 
		   int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
	
    void doDuties();
    bool check();
    unsigned long getTimeout();
    ~TSrvMsgRequest();
    string getName();

  private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
};


#endif /* SRVMSGREQUEST_H */
