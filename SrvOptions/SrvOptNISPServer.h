/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptNISPServer.h,v 1.2 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
 */

#ifndef SRVOPTNISPSERVERS_H
#define SRVOPTNISPSERVERS_H

#include "OptAddrLst.h"

class TSrvOptNISPServers : public TOptAddrLst
{
 public:
    TSrvOptNISPServers(List(TIPv6Addr) lst, TMsg* parent);
    TSrvOptNISPServers(char* buf, int size, TMsg* parent);
    bool doDuties();
};
#endif
