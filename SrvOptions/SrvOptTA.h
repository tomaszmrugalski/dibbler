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
    TSrvOptTA(SPtr<TSrvOptTA> queryOpt, SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,  int iface, int msgType, TMsg* parent);
    TSrvOptTA(char * buf, int bufsize, TMsg* parent);
    TSrvOptTA(int iaid, int statusCode, string txt, TMsg* parent);
    /// @todo: Why 3 construstors?
    void releaseAllAddrs(bool quiet);

    bool doDuties();
 private:
    SPtr<TIPv6Addr>   ClntAddr;
    SPtr<TDUID>       ClntDuid;
    int                   Iface;
    
    SPtr<TSrvOptIAAddress> assignAddr();
    void solicit(SPtr<TSrvOptTA> queryOpt);
    void request(SPtr<TSrvOptTA> queryOpt);
    void release(SPtr<TSrvOptTA> queryOpt);
    void confirm(SPtr<TSrvOptTA> queryOpt);

    void solicitRequest(SPtr<TSrvOptTA> queryOpt, bool solicit);

    int OrgMessage; // original message, which we are responding to 
};

#endif
