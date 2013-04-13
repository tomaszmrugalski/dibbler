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

TOptAAAAuthentication::TOptAAAAuthentication(char* buf, int n, TMsg* parent)
    :TOpt(OPTION_AAAAUTH, parent)
{
    if (n<4) {
        buf += n;
        n = 0;
        return;
    }
    uint32_t spi = readUint32(buf);
    setAAASPI(spi);
    if (parent) {
        parent->setSPI(spi);
        parent->loadAuthKey();
    }
    buf += sizeof(uint32_t); n -= sizeof(uint32_t);

    Log(Debug) << "Auth: received AAA-SPI: " << std::hex << AAASPI_ << std::dec << LogEnd;

    Valid = true;
}

TOptAAAAuthentication::TOptAAAAuthentication(uint32_t spi, TMsg* parent)
    :TOpt(OPTION_AAAAUTH, parent), AAASPI_(spi) {
}

 void TOptAAAAuthentication::setAAASPI( uint32_t value)
{
    AAASPI_ = value;
}

uint32_t TOptAAAAuthentication::getAAASPI() {
    return AAASPI_;
}

size_t TOptAAAAuthentication::getSize() {
    return OPTION6_HDR_LEN + 4;
}

char * TOptAAAAuthentication::storeSelf(char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - OPTION6_HDR_LEN);
    buf = writeUint32(buf, AAASPI_);

    return buf;
}

bool TOptAAAAuthentication::doDuties() {
    return true;
}
