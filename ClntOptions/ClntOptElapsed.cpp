/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptElapsed.cpp,v 1.8 2008-08-29 00:07:28 thomson Exp $
 *
 */

#include "Portable.h"
#include "DHCPConst.h"
#include "ClntOptElapsed.h"
#include "Logger.h"

TClntOptElapsed::TClntOptElapsed( char * buf,  int n, TMsg* parent)
    :TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, buf,n, parent)
{
    Timestamp = (uint32_t)time(NULL);
}

TClntOptElapsed::TClntOptElapsed(TMsg* parent)
    :TOptInteger(OPTION_ELAPSED_TIME, OPTION_ELAPSED_TIME_LEN, 0, parent)
{
    Timestamp = (uint32_t)time(NULL);
}

bool TClntOptElapsed::doDuties()
{
    return false;
}

char * TClntOptElapsed::storeSelf(char* buf)
{
    Value = (unsigned int)((uint32_t)time(NULL) - Timestamp)*100;
    return TOptInteger::storeSelf(buf);
}
