/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptDomainName.h,v 1.3 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTOPTDOMAINNAME_H
#define CLNTOPTDOMAINNAME_H

#include <iostream>
#include "OptStringLst.h"
#include "DUID.h"
#include "SmartPtr.h"

class TClntOptDomainName : public TOptStringLst
{
 public:
    TClntOptDomainName(List(string) * domains, TMsg* parent);
    TClntOptDomainName(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
