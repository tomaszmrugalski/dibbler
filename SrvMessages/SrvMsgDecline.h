/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */

class TSrvMsgDecline;
#ifndef SRVMSGDECLINE_H
#define SRVMSGDECLINE_H
#include "SrvMsg.h"

class TSrvMsgDecline : public TSrvMsg
{
  public:
    TSrvMsgDecline(SPtr<TSrvIfaceMgr> IfaceMgr,
		   SPtr<TSrvTransMgr> TransMgr,
		   SPtr<TSrvCfgMgr> CfgMgr,
		   SPtr<TSrvAddrMgr> AddrMgr,
		   int iface, SPtr<TIPv6Addr> addr,
		   char* buf, int bufSize);
    
    bool  check();
    string getName();
    void  doDuties();
    unsigned long  getTimeout();
    ~TSrvMsgDecline();
};

#endif /* SRVMSGDECLINE_H */
