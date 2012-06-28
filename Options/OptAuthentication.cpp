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
    :TOpt(OPTION_AUTH, parent), Valid_(false) {
    AuthInfoLen_ = getDigestSize(parent->DigestType);

    if (n < 13) {
        buf += n;
        n = 0;
        return;
    }
    setRDM(*buf);
    buf += 1;
    n -= 1;

    Parent->setReplayDetection(readUint64(buf));
    buf += sizeof(uint64_t);
    n -= sizeof(uint64_t);

    Parent->setSPI(readUint32(buf));
    buf += sizeof(uint32_t);
    n -= sizeof(uint32_t);

    if (n != AuthInfoLen_){
        buf += n;
        n = 0;
        return;
    }

    Parent->setAuthInfoPtr(buf);

    if (Parent->getType() != ADVERTISE_MSG)
        Parent->setAuthInfoKey(Parent->AuthKeys->Get(Parent->getSPI()));

    PrintHex("Received digest: ", buf, AuthInfoLen_);

    buf += n;
    n = 0;

    Valid = true;
}

TOptAuthentication::TOptAuthentication(TMsg* parent)
    :TOpt(OPTION_AUTH, parent), Valid_(true) {
    AuthInfoLen_ = getDigestSize(parent->DigestType);
}

void TOptAuthentication::setRDM( uint8_t value) {
    RDM_ = value;
}

size_t TOptAuthentication::getSize() {
    return 17 + AuthInfoLen_;
}

char * TOptAuthentication::storeSelf( char* buf) {
    AuthInfoLen_ = getDigestSize(Parent->DigestType);
    uint32_t spi = Parent->getSPI();
    uint32_t aaaspi = Parent->getAAASPI();

    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    *buf = RDM_;
    buf+=1;
    buf = writeUint64(buf, Parent->getReplayDetection());
    buf = writeUint32(buf, spi);

    memset(buf, 0, AuthInfoLen_);

    Parent->setAuthInfoPtr(buf);

    Parent->setAuthInfoKey(Parent->AuthKeys->Get(spi));

    // check if we should calculate the key
    if (Parent->getAuthInfoKey() == NULL && Parent->getType() == REQUEST_MSG) {
        Parent->setAuthInfoKey();
        if (spi && aaaspi)
            Parent->AuthKeys->Add(spi, aaaspi, Parent->getAuthInfoKey());
    }

    buf += AuthInfoLen_;

    return buf;
}
