/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptServerUnicast.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    memcpy(this->Addr,buf,16);
    buf-=16; n-=16;
}

int TOptServerUnicast::getSize()
{
    return 20;
}

SmartPtr<TIPv6Addr> TOptServerUnicast::getAddr()
{
    return Addr;
}

char * TOptServerUnicast::storeSelf(char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    Addr=new TIPv6Addr(buf);
	buf+=16;
	return buf;
}
