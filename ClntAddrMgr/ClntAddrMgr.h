/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntAddrMgr.h,v 1.4 2004-10-27 22:07:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/09/08 21:22:45  thomson
 * Parser improvements, signed/unsigned issues addressed.
 *
 * Revision 1.2  2004/09/07 22:02:32  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 */

#ifndef CLNTADDRMGR_H
#define CLNTADDRMGR_H

#include "Container.h"
#include "SmartPtr.h"
#include "AddrIA.h"
#include "AddrMgr.h"
#include "ClntCfgMgr.h"

class TClntCfgMgr;

class TClntAddrMgr : public TAddrMgr
{
  public:
    TClntAddrMgr(SmartPtr<TClntCfgMgr> ClntConfMgr, string addrdb, bool loadDB);

    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();

    unsigned long getTimeout();
    unsigned long getTentativeTimeout();
    void firstIA();
    SmartPtr<TAddrIA> getIA();
    SmartPtr<TAddrIA> getIA(unsigned long IAID);
    void addIA(SmartPtr<TAddrIA> ptr);
    bool delIA(long IAID);
    int countIA();
    ~TClntAddrMgr();

    void doDuties();
    
    bool isIAAssigned(unsigned long IAID);
 private:
    SmartPtr<TAddrClient> Client;
};

#endif
