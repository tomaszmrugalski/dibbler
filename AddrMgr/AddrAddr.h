/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef ADDRADDR_H_HEADER_INCLUDED_C1122093
#define ADDRADDR_H_HEADER_INCLUDED_C1122093

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

    long getPref();
    long getValid();

    // return address in packed format (char[16])
    SmartPtr<TIPv6Addr> get();

    // return Prefered time left
    long getPrefTimeout();
    void setPref(long pref);
    void setValid(long valid);

    // return Valid time left
    long getValidTimeout();

    // return timestamp
    long getTimestamp();

    // set timestamp
    void setTimestamp(long ts);
    void setTimestamp();        // now
    
    // tentative
    enum ETentative getTentative();
    void setTentative(enum ETentative state);

  private:
    enum ETentative Tentative;

    long Prefered;
    long Valid;
    SmartPtr<TIPv6Addr> Addr;
    long Timestamp;
};


#endif

