/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptNISServer.h,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTNISSERVERS_H
#define SRVOPTNISSERVERS_H

#include "OptAddrLst.h"

class TSrvOptNISServers : public TOptAddrLst
{
 public:
    TSrvOptNISServers(List(TIPv6Addr) lst, TMsg* parent);
    TSrvOptNISServers(char* buf, int size, TMsg* parent);
    bool doDuties();
};
#endif
