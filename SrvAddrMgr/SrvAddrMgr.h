/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvAddrMgr.h,v 1.3 2004-06-20 21:00:26 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 *                                                                           
 */

class TSrvAddrMgr;
#ifndef SRVADDRMGR_H
#define SRVADDRMGR_H

#include "AddrMgr.h"
#include "SrvCfgAddrClass.h"
class TSrvAddrMgr : public TAddrMgr
{
  public:
    TSrvAddrMgr(string addrdb, bool loadDB=false);

    long getTimeout();
    bool addClntAddr(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr,
		     int iface, long IAID, unsigned long T1, unsigned long T2, 
		     SmartPtr<TIPv6Addr> addr, long pref, long valid,
		     bool quiet);
    bool delClntAddr(SmartPtr<TDUID> duid,long IAID, SmartPtr<TIPv6Addr> addr,
		     bool quiet);
    
    // how many addresses does this client have?
    unsigned long getAddrCount(SmartPtr<TDUID> duid, int iface);

    void doDuties();
    
    void getAddrsCount(
        SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > classes,
        long    *clntCnt,
        long    *addrCnt,
        SmartPtr<TDUID> duid, 
        int iface);

    bool addrIsFree(SmartPtr<TIPv6Addr> addr);
};

#endif
