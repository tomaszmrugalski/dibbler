/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptKeyGeneration.cpp,v 1.2 2008-06-18 23:22:14 thomson Exp $
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include <stdlib.h>
#include "OptKeyGeneration.h"
#include "DHCPConst.h"
#include "Portable.h"
#include "Portable.h"
#include "Msg.h"
#include "Portable.h"
#include "Logger.h"

TOptKeyGeneration::TOptKeyGeneration( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_KEYGEN, parent)
{
    Valid=true;
    if (n<13)
    {
        Valid=false;
        buf+=n;
        n=0;
        return;
    }
    this->Parent->setSPI(ntohl(*(uint32_t*)buf));
    buf +=4; n -=4;

    this->setLifetime(ntohl(*(uint32_t*)buf));
    buf +=4; n -=4;

    this->Parent->setAAASPI(ntohl(*(uint32_t*)buf));
    buf +=4; n -=4;

    this->setAlgorithmId(ntohs(*(uint16_t*)buf));
    buf +=2; n -=2;

    this->Parent->setKeyGenNonce(buf, n);

    PrintHex("KeyGenNonce: ", buf, n);
    
    buf+=n; n = 0;

    this->Parent->setAuthInfoKey();
    this->Parent->AuthKeys->Add(this->Parent->getSPI(), this->Parent->getAAASPI(), this->Parent->getAuthInfoKey());
}

TOptKeyGeneration::TOptKeyGeneration(TMsg* parent)
	:TOpt(OPTION_KEYGEN, parent)
{
}

 void TOptKeyGeneration::setLifetime( uint32_t value)
{
	Lifetime = value;
}

 void TOptKeyGeneration::setAlgorithmId( uint16_t value)
{
	AlgorithmId = value;
}

 uint16_t TOptKeyGeneration::getAlgorithmId()
{
	return AlgorithmId;
}

 int TOptKeyGeneration::getSize()
{
	return 18 + this->Parent->getKeyGenNonceLen();
}

 char * TOptKeyGeneration::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(getSize() - 4);
    buf+=2;
    *(uint32_t*)buf = htonl(this->Parent->getSPI());
    buf+=4;
    *(uint32_t*)buf = htonl(Lifetime);
    buf+=4;
    *(uint32_t*)buf = htonl(this->Parent->getAAASPI());
    buf+=4;
    *(uint16_t*)buf = htons(AlgorithmId);
    buf+=2;

    Log(Debug) << "Auth:Key Generation Nonce length: " << this->Parent->getKeyGenNonceLen() << LogEnd;
    if (this->Parent->getKeyGenNonceLen()) {
        memcpy(buf, this->Parent->getKeyGenNonce(), this->Parent->getKeyGenNonceLen());
        buf+=this->Parent->getKeyGenNonceLen();
    }

    if (0 == this->Parent->setAuthInfoKey())
        this->Parent->AuthKeys->Add(this->Parent->getSPI(), this->Parent->getAAASPI(), this->Parent->getAuthInfoKey());

    return buf;
}

TOptKeyGeneration::~TOptKeyGeneration()
{
/// @todo: do it in Msg.cpp    
//    if (KeyGenNonceLen)
//        delete [] KeyGenNonce;
}
