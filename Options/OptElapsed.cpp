/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptElapsed.cpp,v 1.3 2004-09-28 20:08:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif

#include <time.h>
#include "DHCPConst.h"
#include "Portable.h"
#include "OptElapsed.h"


TOptElapsed::TOptElapsed( char * &buf,  int &bufSize, TMsg* parent)
    :TOpt(OPTION_ELAPSED_TIME, parent)
{
    this->Timestamp=ntohs(*((short*)buf));
    buf-=2; bufSize-=2;
}

TOptElapsed::TOptElapsed(TMsg* parent)
    :TOpt(OPTION_ELAPSED_TIME, parent)
{
    Timestamp = now();
}

int TOptElapsed::getSize()
{
    return 6;
}

char * TOptElapsed::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    *(uint16_t*)buf = htons( now()-this->Timestamp);
    buf+=2;
    return buf;
}
