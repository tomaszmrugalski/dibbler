/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntAddrMgr.h,v 1.9 2006-10-06 00:31:18 thomson Exp $
 *
 */

#ifndef CLNTADDRMGR_H
#define CLNTADDRMGR_H

#include "Container.h"
#include "SmartPtr.h"
#include "AddrIA.h"
#include "AddrMgr.h"
#include "ClntCfgMgr.h"

class TClntAddrMgr : public TAddrMgr
{
  public:
    TClntAddrMgr(SmartPtr<TClntCfgMgr> ClntCfgMgr, string xmlFile, bool loadDB);

    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();

    unsigned long getTimeout();
    unsigned long getTentativeTimeout();

    // --- IA ---
    void firstIA();
    SmartPtr<TAddrIA> getIA();
    SmartPtr<TAddrIA> getIA(unsigned long IAID);
    void addIA(SmartPtr<TAddrIA> ptr);
    bool delIA(long IAID);
    int countIA();

    // --- PD --- 
    void firstPD();
    SmartPtr<TAddrIA> getPD();
    SmartPtr<TAddrIA> getPD(unsigned long IAID);
    void addPD(SmartPtr<TAddrIA> ptr);
    bool delPD(long IAID);
    int countPD();

    // --- TA ---
    void firstTA();
    SmartPtr<TAddrIA> getTA();
    SmartPtr<TAddrIA> getTA(unsigned long iaid);
    void addTA(SmartPtr<TAddrIA> ptr);
    bool delTA(unsigned long iaid);
    int countTA();

    ~TClntAddrMgr();

    void doDuties();
    
    bool isIAAssigned(unsigned long IAID);
    bool isPDAssigned(unsigned long IAID);
 protected:
    void print(ostream &x);
 private:
    SmartPtr<TAddrClient> Client;
};

#endif
