/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptDNSServers.h,v 1.4 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTOPTDNSSERVERS_H
#define CLNTOPTDNSSERVERS_H

#include "OptAddrLst.h"
#include "DUID.h"
#include "IPv6Addr.h"
#include "SmartPtr.h"
#include "Msg.h"

class TClntOptDNSServers : public TOptAddrLst
{
public:
    TClntOptDNSServers( List(TIPv6Addr) * lst, TMsg* parent);
    TClntOptDNSServers(char* buf, int size, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
