/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptSIPServer.h,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTSIPSERVERS_H
#define SRVOPTSIPSERVERS_H

#include "OptAddrLst.h"

class TSrvOptSIPServers : public TOptAddrLst
{
 public:
    TSrvOptSIPServers(List(TIPv6Addr) lst, TMsg* parent);
    TSrvOptSIPServers(char* buf, int size, TMsg* parent);
    bool doDuties();
};
#endif
