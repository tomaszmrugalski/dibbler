/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptElapsed.cpp,v 1.6 2006-10-29 13:11:46 thomson Exp $
 *
 */

#include "DHCPConst.h"
#include "ClntOptElapsed.h"

TClntOptElapsed::TClntOptElapsed( char * buf,  int n, TMsg* parent)
    :TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, buf,n, parent)
{
}

TClntOptElapsed::TClntOptElapsed(TMsg* parent)
    :TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, 0, parent){
}
bool TClntOptElapsed::doDuties()
{
    return false;
}
