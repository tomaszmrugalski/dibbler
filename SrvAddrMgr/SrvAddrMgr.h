/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvAddrMgr.h,v 1.6.2.1 2006-02-05 23:38:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/12/07 00:45:10  thomson
 * Manager creation unified and cleaned up.
 *
 * Revision 1.5  2004/09/07 22:02:33  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.4  2004/06/21 23:08:48  thomson
 * Minor fixes.
 *
 * Revision 1.3  2004/06/20 21:00:26  thomson
 * quiet flag added.
 *
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
    TSrvAddrMgr(string xmlfile);
    ~TSrvAddrMgr();

    long getTimeout();
    // IA address management
    bool addClntAddr(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr,
		     int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
		     SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid,
		     bool quiet);
    bool delClntAddr(SmartPtr<TDUID> duid,unsigned long IAID, SmartPtr<TIPv6Addr> addr,
		     bool quiet);

    // TA address management
    bool addTAAddr(SmartPtr<TDUID> clntDuid, SmartPtr<TIPv6Addr> clntAddr,
		   int iface, unsigned long iaid, SmartPtr<TIPv6Addr> addr, 
		   unsigned long pref, unsigned long valid);
    bool delTAAddr(SmartPtr<TDUID> duid,unsigned long iaid, SmartPtr<TIPv6Addr> addr);
    
    // how many addresses does this client have?
    unsigned long getAddrCount(SmartPtr<TDUID> duid);

    void doDuties();
    
    void getAddrsCount(SmartPtr<List(TSrvCfgAddrClass)> classes, long *clntCnt,
		       long *addrCnt, SmartPtr<TDUID> duid, int iface);

    bool addrIsFree(SmartPtr<TIPv6Addr> addr);
    bool taAddrIsFree(SmartPtr<TIPv6Addr> addr);
};

#endif
