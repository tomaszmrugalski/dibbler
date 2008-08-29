/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

class TSrvOptTA;
#ifndef SRVOPTTA_H
#define SRVOPTTA_H

#include "OptTA.h"
#include "SrvOptIAAddress.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"
#include "Container.h"
#include "IPv6Addr.h"

class TSrvOptTA : public TOptTA
{
  public:
/* Constructor used in answers to: SOLICIT, SOLICIT (with RAPID_COMMIT) and REQUEST */
    TSrvOptTA(SmartPtr<TSrvAddrMgr> addrMgr,  SmartPtr<TSrvCfgMgr> cfgMgr,
		 SmartPtr<TSrvOptTA> queryOpt,
		 SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr, 
		 int iface, int msgType, TMsg* parent);
    TSrvOptTA(char * buf, int bufsize, TMsg* parent);
    TSrvOptTA(int iaid, int statusCode, string txt, TMsg* parent);
    void releaseAllAddrs(bool quiet);

    bool doDuties();
 private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TIPv6Addr>   ClntAddr;
    SmartPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    SmartPtr<TSrvOptIAAddress> assignAddr();
    void solicit(SmartPtr<TSrvOptTA> queryOpt);
    void request(SmartPtr<TSrvOptTA> queryOpt);
    void release(SmartPtr<TSrvOptTA> queryOpt);
    void confirm(SmartPtr<TSrvOptTA> queryOpt);

    void solicitRequest(SmartPtr<TSrvOptTA> queryOpt, bool solicit);

    int OrgMessage; // original message, which we are responding to 
};

#endif
/*
 * $Id: SrvOptTA.h,v 1.4 2008-08-29 00:07:37 thomson Exp $
 */
