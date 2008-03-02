/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptGeneric.cpp,v 1.3 2008-03-02 19:32:28 thomson Exp $
 *
 */

#include "RelOptGeneric.h"

TRelOptGeneric::TRelOptGeneric(int type, char* buf, int bufsize, TMsg* parent) 
    :TOptGeneric(type, buf, bufsize, parent){
}

bool TRelOptGeneric::doDuties() {
    return true;
}
