/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptGeneric.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef RELOPTGENERIC_H
#define RELOPTGENERIC_H

#include "DHCPConst.h"
#include "OptGeneric.h"

class TRelOptGeneric : public TOptGeneric
{
 public:
    TRelOptGeneric(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif /* RELOPTGENERIC_H */
