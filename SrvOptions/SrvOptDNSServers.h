/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptDNSServers.h,v 1.3 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTDNSSERVERS_H
#define SRVOPTDNSSERVERS_H

#include "OptAddrLst.h"

class TSrvOptDNSServers : public TOptAddrLst
{
 public:
    TSrvOptDNSServers(List(TIPv6Addr) lst, TMsg* parent);
    TSrvOptDNSServers(char* buf, int size, TMsg* parent);
    bool doDuties();
};
#endif
