/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptGeneric.cpp,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "RelOptGeneric.h"

TRelOptGeneric::TRelOptGeneric(char* buf, int bufsize, TMsg* parent) 
    :TOptGeneric(buf[0]*256+buf[0], buf, bufsize, parent){
}

bool TRelOptGeneric::doDuties() {
    return true;
}
