/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

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
    this->Parent->setSPI(readUint32(buf));
    buf += sizeof(uint32_t); n -= sizeof(uint32_t);

    this->setLifetime(readUint32(buf));
    buf += sizeof(uint32_t); n -= sizeof(uint32_t);

    this->Parent->setAAASPI(readUint32(buf));
    buf += sizeof(uint32_t); n -= sizeof(uint32_t);

    this->setAlgorithmId(readUint16(buf));
    buf += sizeof(uint16_t); n -= sizeof(uint16_t);

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
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    buf = writeUint32(buf, this->Parent->getSPI());
    buf = writeUint32(buf, Lifetime);
    buf = writeUint32(buf, this->Parent->getAAASPI());
    buf = writeUint16(buf, AlgorithmId);

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
