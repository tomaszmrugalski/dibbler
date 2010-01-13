/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptAAAAuthentication.cpp,v 1.2 2008-06-18 23:22:14 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2008-02-25 20:42:46  thomson
 * Missing new AUTH files added.
 *
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include <stdlib.h>
#include "OptAAAAuthentication.h"
#include "DHCPConst.h"
#include "Portable.h"
#include "Portable.h"
#include "Logger.h"
#include "Msg.h"
#include "Portable.h"

// in AAAAUTH we always use HMAC-SHA1 
#define AAAAuthInfoLen 20

TOptAAAAuthentication::TOptAAAAuthentication( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_AAAAUTH, parent)
{
    Valid=true;
    if (n<4)
    {
        Valid=false;
        buf+=n;
        n=0;
        return;
    }
    this->setAAASPI(ntohl(*(uint32_t*)buf));
    this->Parent->setAAASPI(ntohl(*(uint32_t*)buf));

    buf +=4; n -=4;

    if (n != AAAAuthInfoLen)
    {
	Log(Warning) << "Auth: Malformed AAAAUTH option received: length= " << n << ", expected " << AAAAuthInfoLen << LogEnd;
        Valid=false;
        buf+=n;
        n=0;
        return;
    }

    this->Parent->setAuthInfoPtr(buf);

    PrintHex("Auth: received digest: ", buf, AAAAuthInfoLen);
    
    buf+=n; n = 0;

    PrintHex("Auth:received AAA-SPI: ", (char *)&this->AAASPI, sizeof(this->AAASPI));

    this->Parent->setAuthInfoKey();
}

TOptAAAAuthentication::TOptAAAAuthentication(TMsg* parent)
	:TOpt(OPTION_AAAAUTH, parent)
{
}

 void TOptAAAAuthentication::setAAASPI( uint32_t value)
{
	AAASPI = value;
}

 uint32_t TOptAAAAuthentication::getAAASPI()
{
	return AAASPI;
}

 int TOptAAAAuthentication::getSize()
{
	return 8 + AAAAuthInfoLen;
}

 char * TOptAAAAuthentication::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(getSize() - 4);
    buf+=2;
    *(uint32_t*)buf = htonl(AAASPI);
    buf+=4;

    memset(buf, 0, AAAAuthInfoLen);

    this->Parent->setAuthInfoPtr(buf);
    this->Parent->setAAASPI(AAASPI);

    /// @todo is this neccesary? AuthInfoKey will be different later anyway
    this->Parent->setAuthInfoKey();

    buf+=AAAAuthInfoLen;

    return buf;
}
