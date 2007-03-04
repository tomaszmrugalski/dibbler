/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 * $Id: AddrAddr.h,v 1.7 2007-03-04 20:56:44 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2006-10-06 00:30:16  thomson
 * Initial PD support.
 *
 * Revision 1.5  2006-08-03 00:43:15  thomson
 * FQDN support added.
 *
 * Revision 1.4  2004-06-20 19:36:46  thomson
 * Minor fixes.
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

    // return address in packed format (char[16])
    SmartPtr<TIPv6Addr> get();

    // lifetime related
    unsigned long getPref();
    unsigned long getValid();
    unsigned long getPrefTimeout();
    void setPref(unsigned long pref);
    void setValid(unsigned long valid);
    unsigned long getValidTimeout();

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
};
#endif
