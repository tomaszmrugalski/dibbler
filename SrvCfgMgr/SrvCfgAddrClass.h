/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgAddrClass.h,v 1.13 2005-08-03 22:47:34 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2005/08/02 00:33:58  thomson
 * White-list bug fixed (bug #120),
 * Minor compilation warnings in gcc 4.0 removed.
 *
 * Revision 1.11  2004/12/07 00:43:03  thomson
 * Server no longer support link local addresses (bug #38),
 * Server now supports stateless mode (bug #71)
 *
 * Revision 1.10  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.9  2004/09/08 21:22:45  thomson
 * Parser improvements, signed/unsigned issues addressed.
 *
 * Revision 1.8  2004/09/03 23:20:23  thomson
 * RAPID-COMMIT support fixed. (bugs #50, #51, #52)
 *
 * Revision 1.7  2004/07/05 00:12:30  thomson
 * Lots of minor changes.
 *
 * Revision 1.6  2004/06/29 22:03:36  thomson
 * *MaxLease option partialy implemented/fixed.
 *
 * Revision 1.5  2004/06/28 22:37:59  thomson
 * Minor changes.
 *
 * Revision 1.4  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 *                                                                           
 */

class TSrvCfgAddrClass;
#ifndef SRVCONFADDRCLASS_H
#define SRVCONFADDRCLASS_H

#include <string>
#include <iostream>
#include <iomanip>

#include "SrvAddrMgr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include "DUID.h"
#include "SmartPtr.h"


using namespace std;


class TSrvCfgAddrClass
{
    friend ostream& operator<<(ostream& out,TSrvCfgAddrClass& iface);
 public:
    TSrvCfgAddrClass();
    
    //Is client with this DUID and IP address supported?
    bool clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    //Is client with this DUID and IP address prefered? (is in accept-only?)
    bool clntPrefered(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    
    //checks if the address belongs to the pool
    bool addrInPool(SmartPtr<TIPv6Addr> addr);
    unsigned long countAddrInPool();
    SmartPtr<TIPv6Addr> getRandomAddr();
    
    unsigned long getT1(long clntT1);
    unsigned long getT2(long clntT2);
    unsigned long getPref(long clntPref);
    unsigned long getValid(long clntValid);
    unsigned long getClassMaxLease();
    unsigned long getID();
    unsigned long getShare();

    bool isLinkLocal();

    unsigned long getAssignedCount();
    long incrAssigned(int count=1);
    long decrAssigned(int count=1);

    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    virtual ~TSrvCfgAddrClass();
 private:
    unsigned long T1Beg;
    unsigned long T2Beg;
    unsigned long PrefBeg;
    unsigned long ValidBeg;
    unsigned long T1End;
    unsigned long T2End;
    unsigned long PrefEnd;
    unsigned long ValidEnd;
    unsigned long Share;
    
    long chooseTime(long beg, long end, long clntTime);
    
    unsigned long ID;
    static unsigned long staticID;

    TContainer<SmartPtr<TStationRange> > RejedClnt;
    TContainer<SmartPtr<TStationRange> > AcceptClnt;
    SmartPtr<TStationRange> Pool;
    unsigned long ClassMaxLease;
    unsigned long AddrsAssigned;
    unsigned long AddrsCount;
};

#endif
