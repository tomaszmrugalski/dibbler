/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptSIPServer.h,v 1.1 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTOPTSIPSERVERS_H
#define CLNTOPTSIPSERVERS_H

#include "OptAddrLst.h"
#include "DUID.h"

class TClntOptSIPServers : public TOptAddrLst
{
 public:
    TClntOptSIPServers(List(TIPv6Addr) * lst, TMsg* parent);
    TClntOptSIPServers(char* buf, int size, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
 private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
