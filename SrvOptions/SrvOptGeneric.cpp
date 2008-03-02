/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: SrvOptGeneric.cpp,v 1.1 2008-03-02 19:51:07 thomson Exp $
 *
 */

#include "SrvOptGeneric.h"

TSrvOptGeneric::TSrvOptGeneric(int type, char* buf, int bufsize, TMsg* parent) 
    :TOptGeneric(type, buf, bufsize, parent){
}

bool TSrvOptGeneric::doDuties() {
    return true;
}
