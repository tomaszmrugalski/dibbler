/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptIAAddress.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
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

#include "DHCPConst.h"
#include "Opt.h"
#include "OptIAAddress.h"

TOptIAAddress::TOptIAAddress( char * &buf, int &n, TMsg* parent)
	:TOpt(OPTION_IAADDR, parent)
{
    this->ValidOpt=false;
    if (n>=24)
    {
        Addr=new TIPv6Addr(buf);
        buf+= 16; n-=16;
        this->Pref  = ntohl(*((long*)buf));
        buf+= 4;  n-=4;
        this->Valid = ntohl(*((long*)buf));
        buf+= 4;  n-=4;    
        this->ValidOpt=true;
    }
}

TOptIAAddress::TOptIAAddress( SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent)
	:TOpt(OPTION_IAADDR, parent)

{
    if(addr)
        Addr=addr;
    else
        Addr=new TIPv6Addr();
    Pref = pref;
    Valid = valid;
}


 int TOptIAAddress::getSize()
{
     int mySize = 28;
	return mySize+getSubOptSize();
}


 char * TOptIAAddress::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    memcpy(buf,Addr->getAddr(),16);
    buf+=16;
    *(uint32_t*)buf = htonl(Pref);
    buf+=4;
    *(uint32_t*)buf = htonl(Valid);
    buf+=4;
	buf=storeSubOpt(buf);
    return buf;
}

 SmartPtr<TIPv6Addr> TOptIAAddress::getAddr()
{
	return this->Addr;
}

 long TOptIAAddress::getPref()
{
	return this->Pref;
}

 long TOptIAAddress::getValid()
{
	return this->Valid;
}

bool TOptIAAddress::isValid()
{
    return this->ValidOpt;
}
