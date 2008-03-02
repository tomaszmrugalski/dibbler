/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptGeneric.h,v 1.4 2008-03-02 19:32:28 thomson Exp $
 *
 */

#ifndef RELOPTGENERIC_H
#define RELOPTGENERIC_H

#include "DHCPConst.h"
#include "OptGeneric.h"

class TRelOptGeneric : public TOptGeneric
{
 public:
    TRelOptGeneric(int type, char* buf, int bufsize, TMsg* parent);
    bool doDuties();

};

#endif /* RELOPTGENERIC_H */
