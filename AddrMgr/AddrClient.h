/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrClient.h,v 1.3 2004-09-08 21:22:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

class TAddrClient;
#ifndef ADDRCLIENT_H
#define ADDRCLIENT_H

#include "SmartPtr.h"
#include "Container.h"
#include "AddrIA.h"
#include "DUID.h"

class TAddrClient
{
    friend ostream & operator<<(ostream & strum,TAddrClient &x);

public:
    TAddrClient(SmartPtr<TDUID> duid);
    SmartPtr<TDUID> getDUID();

    //--- IA list ---
    void firstIA();
    SmartPtr<TAddrIA> getIA();
    SmartPtr<TAddrIA> getIA(unsigned long IAID);
    void addIA(SmartPtr<TAddrIA> ia);
    bool delIA(unsigned long IAID);
    int countIA();

    // time related
    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();

private:
    TContainer< SmartPtr<TAddrIA> > IAsLst;
    SmartPtr<TDUID> DUID;
};



#endif 
