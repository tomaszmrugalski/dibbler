/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef ADDRADDR_H
#define ADDRADDR_H

#include <list>
#include <iostream>
#include "IPv6Addr.h"
#include "SmartPtr.h"
#include "DHCPConst.h"

class TAddrAddr
{
    friend std::ostream & operator<<(std::ostream & strum, TAddrAddr &x);
  public:

    TAddrAddr(SPtr<TIPv6Addr> addr, long pref, long valid);
    TAddrAddr(SPtr<TIPv6Addr> addr, long pref, long valid, int prefix);

    // return address in packed format (char[16])
    SPtr<TIPv6Addr> get();

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
    enum EAddrStatus getTentative();
    void setTentative(enum EAddrStatus state);

  protected:
    enum EAddrStatus Tentative;

    unsigned long Prefered;
    unsigned long Valid;
    SPtr<TIPv6Addr> Addr;
    unsigned long Timestamp;
    int Prefix;
};

typedef std::list< SPtr<TIPv6Addr> > TAddrList;

#endif
