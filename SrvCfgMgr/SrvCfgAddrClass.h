/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgAddrClass.h,v 1.4 2004-06-17 23:53:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    
    //How many addresses can be assigned to this particular client?
    long getAddrCount(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    
    //checks if the address belongs to the pool
    bool addrInPool(SmartPtr<TIPv6Addr> addr);
    unsigned long countAddrInPool();
    SmartPtr<TIPv6Addr> getRandomAddr();
    SmartPtr<TIPv6Addr> getFreeAddr(SmartPtr<TSrvAddrMgr> addrMgr,
				    SmartPtr<TDUID> clntDuid,
				    SmartPtr<TIPv6Addr> clntAddr,
				    SmartPtr<TIPv6Addr> hint);
    
    unsigned long getT1(long clntT1);
    unsigned long getT2(long clntT2);
    unsigned long getPref(long clntPref);
    unsigned long getValid(long clntValid);
    unsigned long getMaxLease();
    unsigned long getMaxClientLease();
    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    bool getRapidCommit();
    
    virtual ~TSrvCfgAddrClass();
 private:
    long T1Beg;
    long T2Beg;
    long PrefBeg;
    long ValidBeg;
    long T1End;
    long T2End;
    long PrefEnd;
    long ValidEnd;
    
    long chooseTime(long beg, long end, long clntTime);

    int Prefix;
    int Name;

    TContainer<SmartPtr<TStationRange> > RejedClnt;
    TContainer<SmartPtr<TStationRange> > AcceptClnt;
    TContainer<SmartPtr<TStationRange> > Pool;

    unsigned long ClntMaxLease;
    unsigned long ClassMaxLease;
    unsigned long IfaceMaxLease;
    bool Unicast;		
    bool RapidCommit;	
};

#endif
