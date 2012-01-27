/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 */

#include "Portable.h"
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

    this->Parent->setReplayDetection(readUint64(buf));
    buf += sizeof(uint64_t); n -= sizeof(uint64_t);

    this->Parent->setSPI(readUint32(buf));
    buf += sizeof(uint32_t); n -= sizeof(uint32_t);

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

    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    *buf = RDM;
    buf+=1;
    buf = writeUint64(buf, this->Parent->getReplayDetection());
    buf = writeUint32(buf, spi);

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
