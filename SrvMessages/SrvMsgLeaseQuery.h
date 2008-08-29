/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQuery.h,v 1.2 2008-08-29 00:07:35 thomson Exp $
 *                                                                           
 */

class TSrvMsgLeaseQuery;
#ifndef SRVMSGLEASEQUERY_H
#define SRVMSGLEASEQUERY_H

#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "IPv6Addr.h"

class TSrvMsgLeaseQuery : public TSrvMsg
{
  public:
    TSrvMsgLeaseQuery(SmartPtr<TSrvIfaceMgr> IfMgr, 
		      SmartPtr<TSrvCfgMgr>   ConfMgr, 
		      SmartPtr<TSrvAddrMgr>  AddrMgr, 
		      int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);
    
    void doDuties();
    bool check();
    ~TSrvMsgLeaseQuery();
    string getName();

  private:
};


#endif /* SRVMSGREQUEST_H */
