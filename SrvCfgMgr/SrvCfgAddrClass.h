/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvCfgAddrClass.h,v 1.17 2008-10-12 19:42:45 thomson Exp $
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
#include "SrvCfgClientClass.h"
using namespace std;


class TSrvCfgAddrClass
{
    friend ostream& operator<<(ostream& out,TSrvCfgAddrClass& iface);
 public:
    TSrvCfgAddrClass();

    //Is client with this DUID and IP address supported?
    bool clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr);
    bool clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr, SPtr<TSrvMsg> msg);

    //Is client with this DUID and IP address prefered? (is in accept-only?)
    bool clntPrefered(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr);

    //checks if the address belongs to the pool
    bool addrInPool(SPtr<TIPv6Addr> addr);
    unsigned long countAddrInPool();
    SPtr<TIPv6Addr> getRandomAddr();

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

    void setOptions(SPtr<TSrvParsGlobalOpt> opt);
    SPtr<TSrvOptAddrParams> getAddrParams();

    virtual ~TSrvCfgAddrClass();
    void mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst);

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

    SPtr<TStationRange> Pool;
    unsigned long ClassMaxLease;
    unsigned long AddrsAssigned;
    unsigned long AddrsCount;

    SPtr<TSrvOptAddrParams> AddrParams; // AddrParams - experimental option

    // new, better white/black-list
    unsigned long ID; // client class ID
    static unsigned long staticID;
    List(string) allowLst;
    List(string) denyLst;
    List(TSrvCfgClientClass) allowClientClassLst;
    List(TSrvCfgClientClass) denyClientClassLst;

    // old white/black-list
    List(TStationRange) RejedClnt;
    List(TStationRange) AcceptClnt;
};

#endif
