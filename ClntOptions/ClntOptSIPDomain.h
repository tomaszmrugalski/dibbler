/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptSIPDomain.h,v 1.1 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTOPTSIPDOMAIN_H
#define CLNTOPTSIPDOMAIN_H

#include <iostream>
#include "OptStringLst.h"
#include "DUID.h"
#include "SmartPtr.h"

class TClntOptSIPDomain : public TOptStringLst
{
 public:
    TClntOptSIPDomain(List(string) * domains, TMsg* parent);
    TClntOptSIPDomain(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
