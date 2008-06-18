/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptAuthentication.cpp,v 1.6 2008-06-18 23:22:14 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2008-02-25 17:49:09  thomson
 * Authentication added. Megapatch by Michal Kowalczuk.
 * (small changes by Tomasz Mrugalski)
 *
 * Revision 1.4  2006-11-30 03:17:46  thomson
 * Auth related changes by Sammael.
 *
 * Revision 1.3  2006-11-24 01:33:43  thomson
 * *** empty log message ***
 *
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

    this->Parent->setReplayDetection(ntohll(*(uint64_t*)buf));
    buf +=8; n -=8;

    this->Parent->setSPI(ntohl(*(uint32_t*)buf));
    buf +=4; n -=4;

    if (n != AuthInfoLen)
    {
        Valid=false;
        buf+=n;
        n=0;
        return;
    }

    this->Parent->setAuthInfoPtr(buf);

    if (this->Parent->getType() != ADVERTISE_MSG)
        this->Parent->setAuthInfoKey(this->Parent->AuthKeys->Get(this->Parent->getSPI()));

    PrintHex("Received digest: ", buf, AuthInfoLen);
    
    buf+=n; n = 0;
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

 int TOptAuthentication::getSize()
{
	return 17 + AuthInfoLen;
}

 char * TOptAuthentication::storeSelf( char* buf)
{
    AuthInfoLen = getDigestSize(this->Parent->DigestType);
    uint32_t spi = this->Parent->getSPI();
    uint32_t aaaspi = this->Parent->getAAASPI();

    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(getSize() - 4);
    buf+=2;
    *buf = RDM;
    buf+=1;
    *(uint64_t*)buf = htonll(this->Parent->getReplayDetection());
    buf+=8;
    *(uint32_t*)buf = htonl(spi);
    buf+=4;

    memset(buf, 0, AuthInfoLen);

    this->Parent->setAuthInfoPtr(buf);

    this->Parent->setAuthInfoKey(this->Parent->AuthKeys->Get(spi));

    // check if we should calculate the key
    if (this->Parent->getAuthInfoKey() == NULL && this->Parent->getType() == REQUEST_MSG) {
        this->Parent->setAuthInfoKey();
        if (spi && aaaspi)
            this->Parent->AuthKeys->Add(spi, aaaspi, this->Parent->getAuthInfoKey());
    }

    buf+=AuthInfoLen;

    return buf;
}
