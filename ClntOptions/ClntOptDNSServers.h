/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef CLNTOPTDNSSERVERS_H
#define CLNTOPTDNSSERVERS_H

#include "OptDNSServers.h"
#include "DUID.h"
#include "IPv6Addr.h"
#include "SmartPtr.h"
#include "ClntMsg.h"

class TClntOptDNSServers : public TOptDNSServers
{
public:
    TClntOptDNSServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent);
    TClntOptDNSServers(char* buf, int size, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
