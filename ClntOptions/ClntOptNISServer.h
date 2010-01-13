/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptNISServer.h,v 1.2 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-10-25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#ifndef CLNTOPTNISSERVERS_H
#define CLNTOPTNISSERVERS_H

#include "OptAddrLst.h"
#include "DUID.h"

class TClntOptNISServers : public TOptAddrLst
{
 public:
    TClntOptNISServers(List(TIPv6Addr) *lst, TMsg* parent);
    TClntOptNISServers(char* buf, int size, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SPtr<TDUID> duid);
 private:
    SPtr<TDUID> SrvDUID;
};
#endif
