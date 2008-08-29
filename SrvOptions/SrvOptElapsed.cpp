/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptElapsed.cpp,v 1.5 2008-08-29 00:07:36 thomson Exp $
 */

#include "DHCPConst.h"
#include "SrvOptElapsed.h"

TSrvOptElapsed::TSrvOptElapsed( char * buf,  int n, TMsg* parent)
    :TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, buf,n, parent) {
    
}

TSrvOptElapsed::TSrvOptElapsed(TMsg* parent)
    :TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, 0, parent) {

}

bool TSrvOptElapsed::doDuties() {
    return true;
}
