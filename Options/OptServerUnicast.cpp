/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptServerUnicast.cpp,v 1.4 2004-09-07 17:42:31 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 */
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "OptServerUnicast.h"

TOptServerUnicast::TOptServerUnicast( char * &buf,  int &n, TMsg* parent)
    :TOpt(OPTION_UNICAST, parent)
{
    this->Addr = new TIPv6Addr(buf, false); // plain = false
    buf-=16; n-=16;
}

TOptServerUnicast::TOptServerUnicast(SmartPtr<TIPv6Addr> addr, TMsg* parent) 
    :TOpt(OPTION_UNICAST, parent)
{
    this->Addr = addr;
}

int TOptServerUnicast::getSize() {
    return 20;
}

SmartPtr<TIPv6Addr> TOptServerUnicast::getAddr()
{
    return Addr;
}

char * TOptServerUnicast::storeSelf(char* buf) {
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    this->Addr->storeSelf(buf);
    return buf;
}
