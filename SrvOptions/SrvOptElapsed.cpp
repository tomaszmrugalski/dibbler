/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptElapsed.cpp,v 1.2 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include <stdlib.h>
#include <time.h>
#include "DHCPConst.h"
#include "Portable.h"
#include "SrvOptElapsed.h"

TSrvOptElapsed::TSrvOptElapsed( char * buf,  int n, TMsg* parent)
    :TOptElapsed(buf,n, parent) {
    
}

TSrvOptElapsed::TSrvOptElapsed(TMsg* parent)
    :TOptElapsed(parent) {

}

bool TSrvOptElapsed::doDuties() {
    return true;
}
