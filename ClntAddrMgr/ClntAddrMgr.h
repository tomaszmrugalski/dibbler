/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntAddrMgr.h,v 1.14 2008-10-12 14:05:25 thomson Exp $
 *
 */

#ifndef CLNTADDRMGR_H
#define CLNTADDRMGR_H

#include "Container.h"
#include "SmartPtr.h"
#include "AddrIA.h"
#include "AddrMgr.h"
#include "Portable.h"
#include "IPv6Addr.h"

#define ClntAddrMgr() (TClntAddrMgr::instance())

class TClntAddrMgr : public TAddrMgr
{
 private:
    TClntAddrMgr(SPtr<TDUID> clientDuid, bool useConfirm, string xmlFile, bool loadDB);

  public:
    static TClntAddrMgr& instance();
    static void instanceCreate(SPtr<TDUID> clientDUID, bool useConfirm, string xmlFile, bool loadDB);

    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();

    unsigned long getTimeout();
    unsigned long getTentativeTimeout();

    // --- IA ---
    void firstIA();
    SPtr<TAddrIA> getIA();
    SPtr<TAddrIA> getIA(unsigned long IAID);
    void addIA(SPtr<TAddrIA> ptr);
    bool delIA(long IAID);
    int countIA();
    void setIA2Confirm(volatile link_state_notify_t * changedLinks);
    SPtr<TIPv6Addr> getPreferredAddr();

    // --- PD --- 
    void firstPD();
    SPtr<TAddrIA> getPD();
    SPtr<TAddrIA> getPD(unsigned long IAID);
    void addPD(SPtr<TAddrIA> ptr);
    bool delPD(long IAID);
    int countPD();
    bool addPrefix(SPtr<TDUID> srvDuid , SPtr<TIPv6Addr> srvAddr,
		   int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
		   SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
		   int length, bool quiet);
    bool updatePrefix(SPtr<TDUID> srvDuid , SPtr<TIPv6Addr> srvAddr,
		      int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
		      SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
		      int length, bool quiet);

    // --- TA ---
    void firstTA();
    SPtr<TAddrIA> getTA();
    SPtr<TAddrIA> getTA(unsigned long iaid);
    void addTA(SPtr<TAddrIA> ptr);
    bool delTA(unsigned long iaid);
    int countTA();

    ~TClntAddrMgr();

    void doDuties();
    
 protected:
    void print(ostream &x);
 private:
    SPtr<TAddrClient> Client;
    static TClntAddrMgr * Instance;
};

#endif
