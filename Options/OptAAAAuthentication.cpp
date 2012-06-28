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
#include "Portable.h"
#include "OptAAAAuthentication.h"
#include "DHCPConst.h"
#include "Portable.h"
#include "Portable.h"
#include "Logger.h"
#include "Msg.h"
#include "Portable.h"

// in AAAAUTH we always use HMAC-SHA1 
#define AAA_AUTH_INFO_LEN 20

TOptAAAAuthentication::TOptAAAAuthentication( char * &buf,  int &n, TMsg* parent)
    :TOpt(OPTION_AAAAUTH, parent)
{
    if (n<4) {
        buf += n;
        n = 0;
        return;
    }
    setAAASPI(readUint32(buf));
    Parent->setAAASPI(readUint32(buf));
    buf += sizeof(uint32_t); n -= sizeof(uint32_t);

    if (n != AAA_AUTH_INFO_LEN)
    {
	Log(Warning) << "Auth: Malformed AAAAUTH option received: length="
                     << n << ", expected " << AAA_AUTH_INFO_LEN << LogEnd;
        buf += n;
        n = 0;
        return;
    }

    Parent->setAuthInfoPtr(buf);

    PrintHex("Auth: received digest: ", buf, AAA_AUTH_INFO_LEN);
    
    buf+=n; n = 0;

    PrintHex("Auth:received AAA-SPI: ", (char *)&AAASPI_, sizeof(AAASPI_));

    Parent->setAuthInfoKey();

    Valid = true;
}

TOptAAAAuthentication::TOptAAAAuthentication(TMsg* parent)
    :TOpt(OPTION_AAAAUTH, parent) {
    Valid = true;
}


 void TOptAAAAuthentication::setAAASPI( uint32_t value)
{
	AAASPI_ = value;
}

uint32_t TOptAAAAuthentication::getAAASPI() {
	return AAASPI_;
}

size_t TOptAAAAuthentication::getSize() {
	return 8 + AAA_AUTH_INFO_LEN;
}

char * TOptAAAAuthentication::storeSelf(char* buf) {
    if (!Parent) {
        Log(Error) << "Attempted to send orphaned AAA Auth option." << LogEnd;
        return buf;
    }
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    buf = writeUint32(buf, AAASPI_);

    memset(buf, 0, AAA_AUTH_INFO_LEN);

    Parent->setAuthInfoPtr(buf);
    Parent->setAAASPI(AAASPI_);

    /// @todo is this neccesary? AuthInfoKey will be different later anyway
    Parent->setAuthInfoKey();

    buf += AAA_AUTH_INFO_LEN;

    return buf;
}
