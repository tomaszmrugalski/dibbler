class TSrvCfgAddrClass;
#ifndef SRVCONFADDRCLASS_H
#define SRVCONFADDRCLASS_H

#include <string>
#include <iostream>
#include <iomanip>

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

    //Is client with this DUID and IP address supported 
    //i.e. can be assigned address ?
    bool clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    //and how many address can be assigned to this particular client 
    //i.e. (0 - ULONG_MAX)
    long getAddrCount(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    //checks if the address belongs to the pool
    bool addrInPool(SmartPtr<TIPv6Addr> addr);
    unsigned long TSrvCfgAddrClass::countAddrInPool();
    SmartPtr<TIPv6Addr> getRandomAddr();

    unsigned long getT1(long clntT1);
    unsigned long getT2(long clntT2);
    unsigned long getPref(long clntPref);
    unsigned long getValid(long clntValid);
    unsigned long getMaxLease();
    unsigned long getMaxClientLease();
    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    bool getRapidCommit();
    char getPreference();

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

    unsigned long MaxClientLease;
    unsigned long MaxLease;
    char Preference;	
    bool Unicast;		
    bool RapidCommit;	
};

#endif
