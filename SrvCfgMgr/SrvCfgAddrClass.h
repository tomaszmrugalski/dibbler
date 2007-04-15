/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgAddrClass.h,v 1.14.2.1 2007-04-15 19:26:31 thomson Exp $
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
#include "SrvOptAddrParams.h"

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
    
    unsigned long getT1(unsigned long clntT1);
    unsigned long getT2(unsigned long clntT2);
    unsigned long getPref(unsigned long clntPref);
    unsigned long getValid(unsigned long clntValid);
    unsigned long getClassMaxLease();
    unsigned long getID();
    unsigned long getShare();

    bool isLinkLocal();

    unsigned long getAssignedCount();
    long incrAssigned(int count=1);
    long decrAssigned(int count=1);

    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    SPtr<TSrvOptAddrParams> getAddrParams();
    
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
    
    long chooseTime(unsigned long beg, unsigned long end, unsigned long clntTime);
    
    unsigned long ID;
    static unsigned long staticID;

    TContainer<SmartPtr<TStationRange> > RejedClnt;
    TContainer<SmartPtr<TStationRange> > AcceptClnt;
    SmartPtr<TStationRange> Pool;
    unsigned long ClassMaxLease;
    unsigned long AddrsAssigned;
    unsigned long AddrsCount;

    SPtr<TSrvOptAddrParams> AddrParams; // AddrParams - experimental option
};

#endif
