/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQuery.h,v 1.1 2007-11-01 08:10:34 thomson Exp $
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
