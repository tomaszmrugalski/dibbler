/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptSIPDomain.h,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef SRVOPTSIPDOMAIN_H
#define SRVOPTSIPDOMAIN_H

#include <iostream>
#include "OptStringLst.h"
#include "DUID.h"
#include "SmartPtr.h"

class TSrvOptSIPDomain : public TOptStringLst
{
 public:
    TSrvOptSIPDomain(List(string) domains, TMsg* parent);
    TSrvOptSIPDomain(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
