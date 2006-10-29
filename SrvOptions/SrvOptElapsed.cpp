/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptElapsed.cpp,v 1.4 2006-10-29 13:11:47 thomson Exp $
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
