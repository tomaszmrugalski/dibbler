/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Nguyen Vinh Nghiem
 *
 * released under GNU GPL v2 or later licence
 *
 */

class TSrvCfgAddrClass;
#ifndef SRVCONFADDRCLASS_H
#define SRVCONFADDRCLASS_H

#include <string>
#include <iostream>
#include <iomanip>

#include "DHCPDefaults.h"
#include "SrvAddrMgr.h"
#include "SrvParsGlobalOpt.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include "DUID.h"
#include "SmartPtr.h"
#include "SrvOptAddrParams.h"
#include "SrvCfgClientClass.h"

class TSrvCfgAddrClass
{
    friend std::ostream& operator<<(std::ostream& out, TSrvCfgAddrClass& iface);
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
    SPtr<TIPv6Addr> getFirstAddr();
    SPtr<TIPv6Addr> getLastAddr();

    uint32_t getT1(uint32_t clntT1 = SERVER_DEFAULT_MAX_T1);
    uint32_t getT2(uint32_t clntT2 = SERVER_DEFAULT_MAX_T2);
    uint32_t getPref(uint32_t clntPref = SERVER_DEFAULT_MAX_PREF);
    uint32_t getValid(uint32_t clntValid = SERVER_DEFAULT_MAX_VALID);
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
    uint32_t T1Min_;
    uint32_t T2Min_;
    uint32_t PrefMin_;
    uint32_t ValidMin_;
    uint32_t T1Max_;
    uint32_t T2Max_;
    uint32_t PrefMax_;
    uint32_t ValidMax_;
    uint32_t Share_;

    uint32_t chooseTime(uint32_t beg, uint32_t end, uint32_t clntTime);

    SPtr<THostRange> Pool_;
    unsigned long ClassMaxLease_;
    unsigned long AddrsAssigned_;
    unsigned long AddrsCount_;

    SPtr<TSrvOptAddrParams> AddrParams_; // AddrParams - experimental option

    // new, better white/black-list
    unsigned long ID_; // client class ID
    static unsigned long StaticID_;
    List(std::string) AllowLst_;
    List(std::string) DenyLst_;
    List(TSrvCfgClientClass) AllowClientClassLst_;
    List(TSrvCfgClientClass) DenyClientClassLst_;

    // old white/black-list
    List(THostRange) RejedClnt_;
    List(THostRange) AcceptClnt_;
};

#endif
