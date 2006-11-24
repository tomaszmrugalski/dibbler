/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptAuthentication.cpp,v 1.3 2006-11-24 01:33:43 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2006-11-17 01:28:38  thomson
 * Partial AUTH support by Sammael, fixes by thomson
 *
 * Revision 1.1  2006-11-17 00:37:16  thomson
 * Partial AUTH support by Sammael, fixes by thomson
 *
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
#include "OptAuthentication.h"
#include "DHCPConst.h"
#include "Portable.h"
#include "Logger.h"
#include "Msg.h"
#include "Portable.h"

TOptAuthentication::TOptAuthentication( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_AUTH, parent)
{
    AuthInfoLen = getDigestSize(parent->DigestType);

    Valid=true;
    if (n<13)
    {
        Valid=false;
        buf+=n;
        n=0;
        return;
    }
    this->setRDM(*buf);
    buf +=1; n -=1;

    this->setReplayDet(ntohll(*(uint64_t*)buf));
    buf +=8; n -=8;

    this->setSPI(ntohl(*(uint32_t*)buf));
    buf +=4; n -=4;

    cout << "n = " << n << ", AuthInfoLen = " << AuthInfoLen << endl;
    if (n != AuthInfoLen)
    {
        Valid=false;
        buf+=n;
        n=0;
        return;
    }

    this->Parent->setAuthInfoPtr(buf);

    cout << "received digest: ";
    printhex(buf, AuthInfoLen);
    
    buf+=n; n = 0;

    cout << (int)this->RDM << endl;
    cout << this->ReplayDet << endl;
    cout << this->SPI << endl;
}

TOptAuthentication::TOptAuthentication(TMsg* parent)
	:TOpt(OPTION_AUTH, parent)
{
    AuthInfoLen = getDigestSize(parent->DigestType);
}

 void TOptAuthentication::setRDM( uint8_t value)
{
    RDM = value;
}

 void TOptAuthentication::setReplayDet( uint64_t value)
{
	ReplayDet = value;
}

 void TOptAuthentication::setSPI( uint32_t value)
{
	SPI = value;
}

 int TOptAuthentication::getSize()
{
	return 17 + AuthInfoLen;
}

 char * TOptAuthentication::storeSelf( char* buf)
{
    printf("[s] Storing AUTH\n");
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(getSize() - 4);
    buf+=2;
    *buf = RDM;
    buf+=1;
    *(uint64_t*)buf = htonll(ReplayDet);
    buf+=8;
    *(uint32_t*)buf = htonl(SPI);
    buf+=4;

    memset(buf, 0, AuthInfoLen);

    this->Parent->setAuthInfoPtr(buf);

    buf+=AuthInfoLen;

    return buf;
}
