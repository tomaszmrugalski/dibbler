/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: SrvOptGeneric.h,v 1.1 2008-03-02 19:51:07 thomson Exp $
 *
 */

#ifndef SRVOPTGENERIC_H
#define SRVOPTGENERIC_H

#include "DHCPConst.h"
#include "OptGeneric.h"

class TSrvOptGeneric : public TOptGeneric
{
 public:
    TSrvOptGeneric(int type, char* buf, int bufsize, TMsg* parent);
    bool doDuties();

};

#endif /* SRVOPTGENERIC_H */
