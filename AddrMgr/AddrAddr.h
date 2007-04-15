/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 * $Id: AddrAddr.h,v 1.7.2.1 2007-04-15 21:23:28 thomson Exp $
 *
 */

#ifndef ADDRADDR_H
#define ADDRADDR_H

#include <iostream>
#include "IPv6Addr.h"
#include "SmartPtr.h"
#include "DHCPConst.h"

using namespace std;

class TAddrAddr
{
    friend ostream & operator<<(ostream & strum,TAddrAddr &x);
  public:

    TAddrAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid);
    TAddrAddr(SmartPtr<TIPv6Addr> addr, long pref, long valid, int prefix);

    // return address in packed format (char[16])
    SmartPtr<TIPv6Addr> get();

    // lifetime related
    unsigned long getPref();
    unsigned long getValid();
    unsigned long getPrefTimeout();
    void setPref(unsigned long pref);
    void setValid(unsigned long valid);
    unsigned long getValidTimeout();
    int getPrefix();

    // timestamp
    long getTimestamp();
    void setTimestamp(long ts);
    void setTimestamp();
    
    // tentative
    enum ETentative getTentative();
    void setTentative(enum ETentative state);

  protected:
    enum ETentative Tentative;

    unsigned long Prefered;
    unsigned long Valid;
    SmartPtr<TIPv6Addr> Addr;
    unsigned long Timestamp;
    int Prefix;
};
#endif
