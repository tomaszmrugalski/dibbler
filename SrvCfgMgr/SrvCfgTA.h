/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 only licence
 *
 */

class TSrcCfgTA;
#ifndef SRVCONFTA_H
#define SRVCONFTA_H

#include <string>
#include <iostream>
#include <iomanip>

#include "SrvAddrMgr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include "DUID.h"

class TSrvCfgTA
{
    friend std::ostream& operator<<(std::ostream& out, TSrvCfgTA& iface);
 public:
    TSrvCfgTA();

    //Is client with this DUID and IP address supported?
    bool clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr);
    //Is client with this DUID and IP address prefered? (is in accept-only?)
    bool clntPrefered(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr);

    unsigned long countAddrInPool();
    SPtr<TIPv6Addr> getRandomAddr();
    bool addrInPool(SPtr<TIPv6Addr> addr);

    unsigned long getPref();
    unsigned long getValid();
    unsigned long getClassMaxLease();
    unsigned long getID();

    unsigned long getAssignedCount();
    long incrAssigned(int count=1);
    long decrAssigned(int count=1);

    void setOptions(SPtr<TSrvParsGlobalOpt> opt);
    virtual ~TSrvCfgTA();

    void mapAllowDenyList( List(TSrvCfgClientClass) clientClassLst);
    bool clntSupported(SPtr<TDUID> duid,SPtr<TIPv6Addr> clntAddr, SPtr<TSrvMsg> msg);
 private:
    unsigned long Pref;
    unsigned long Valid;

    unsigned long ID; // this is not IAID, just internal ID counter
    static unsigned long staticID;

    TContainer<SPtr<THostRange> > RejedClnt;
    TContainer<SPtr<THostRange> > AcceptClnt;
    SPtr<THostRange> Pool;
    unsigned long ClassMaxLease;
    unsigned long AddrsAssigned;
    unsigned long AddrsCount;

    List(std::string) allowLst;
    List(std::string) denyLst;

    List(TSrvCfgClientClass) allowClientClassLst;
    List(TSrvCfgClientClass) denyClientClassLst;
};

#endif
