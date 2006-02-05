/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvCfgTA.h,v 1.1.2.1 2006-02-05 23:42:33 thomson Exp $
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
#include "SmartPtr.h"

using namespace std;


class TSrvCfgTA
{
    friend ostream& operator<<(ostream& out,TSrvCfgTA& iface);
 public:
    TSrvCfgTA();
    
    //Is client with this DUID and IP address supported?
    bool clntSupported(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    //Is client with this DUID and IP address prefered? (is in accept-only?)
    bool clntPrefered(SmartPtr<TDUID> duid,SmartPtr<TIPv6Addr> clntAddr);
    
    unsigned long countAddrInPool();
    SmartPtr<TIPv6Addr> getRandomAddr();
    bool addrInPool(SmartPtr<TIPv6Addr> addr);
    
    unsigned long getPref();
    unsigned long getValid();
    unsigned long getClassMaxLease();
    unsigned long getID();

    unsigned long getAssignedCount();
    long incrAssigned(int count=1);
    long decrAssigned(int count=1);

    void setOptions(SmartPtr<TSrvParsGlobalOpt> opt);
    virtual ~TSrvCfgTA();
 private:
    unsigned long Pref;
    unsigned long Valid;
    
    unsigned long ID; // this is not IAID, just internal ID counter
    static unsigned long staticID;

    TContainer<SmartPtr<TStationRange> > RejedClnt;
    TContainer<SmartPtr<TStationRange> > AcceptClnt;
    SmartPtr<TStationRange> Pool;
    unsigned long ClassMaxLease;
    unsigned long AddrsAssigned;
    unsigned long AddrsCount;
};

#endif

/*
 * $Log: not supported by cvs2svn $
 */
