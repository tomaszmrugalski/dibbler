/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *                              
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgPD.h,v 1.1 2006-10-06 00:35:26 thomson Exp $
 *
 */

class TSrvCfgPD;
#ifndef SRVCONFPD_H
#define SRVCONFPD_H

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
#include "SrvCfgPD.h"

using namespace std;


class TSrvCfgPD
{
    friend ostream& operator<<(ostream& out,TSrvCfgPD& iface);
 public:
    TSrvCfgPD();
    
    //Is client with this DUID and IP address supported?
    bool clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    //Is client with this DUID and IP address prefered? (is in accept-only?)
    bool clntPrefered(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    
    //checks if the prefix belongs to the pool
    bool prefixInPool(SmartPtr<TIPv6Addr> prefix);
    unsigned long countPrefixesInPool();
    SmartPtr<TIPv6Addr> getRandomPrefix();
    SmartPtr<TIPv6Addr>	getFirstAddrInPrefix();
    
    unsigned long getPD_T1(unsigned long clntT1);
    unsigned long getPD_T2(unsigned long clntT2);
    unsigned long getPD_Length(); // length of prefix 
    unsigned long getPD_MaxLease();
    unsigned long getID();
    unsigned long getShare();

    bool isLinkLocal();

    unsigned long getAssignedCount();
    long incrAssigned(int count=1);
    long decrAssigned(int count=1);

    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt, int PDPrefix);
    virtual ~TSrvCfgPD();
 private:
    unsigned long PD_T1Beg;
    unsigned long PD_T2Beg;
    unsigned long PD_T1End;
    unsigned long PD_T2End;
    unsigned long PD_Share;
    unsigned long PD_Length;
    unsigned long PD_Prefix;
    
    long chooseTime(unsigned long beg, unsigned long end, unsigned long clntTime);
    
    unsigned long ID;
    static unsigned long staticID;

    //TContainer<SmartPtr<TStationRange> > RejedClnt;
    //TContainer<SmartPtr<TStationRange> > AcceptClnt;
    SmartPtr<TStationRange> PD_Pool;
    unsigned long PD_MaxLease;
    unsigned long PD_Assigned;
    unsigned long PD_Count;
};

#endif
