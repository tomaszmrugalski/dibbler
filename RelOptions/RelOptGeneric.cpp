/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptGeneric.cpp,v 1.2 2005-01-13 22:45:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 */

#include "RelOptGeneric.h"

TRelOptGeneric::TRelOptGeneric(int type, char* buf, int bufsize, TMsg* parent) 
    :TOptGeneric(type, buf, bufsize, parent){
}

bool TRelOptGeneric::doDuties() {
    return true;
}
