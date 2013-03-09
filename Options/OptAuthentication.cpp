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

TOptAuthentication::TOptAuthentication(char* buf, int buflen, TMsg* parent)
    :TOpt(OPTION_AUTH, parent) {
    AuthInfoLen_ = getDigestSize(parent->DigestType);

    if (buflen < 13) {
        Valid = false;
        return;
    }
    setRDM(static_cast<AuthReplay>(buf[0]));
    buf++;
    buflen--;

    Parent->setReplayDetection(readUint64(buf));
    buf += sizeof(uint64_t);
    buflen -= sizeof(uint64_t);

    Parent->setSPI(readUint32(buf));
    buf += sizeof(uint32_t);
    buflen -= sizeof(uint32_t);

    if (buflen != AuthInfoLen_){
        Valid = false;
        return;
    }

    Parent->setAuthInfoPtr(buf);

    if (Parent->getType() != ADVERTISE_MSG)
        Parent->setAuthInfoKey(Parent->AuthKeys->Get(Parent->getSPI()));

    PrintHex("Received digest: ", buf, AuthInfoLen_);

    Valid = true;
}

TOptAuthentication::TOptAuthentication(AuthProtocols proto, uint8_t algo, AuthReplay rdm, TMsg* parent)
    :TOpt(OPTION_AUTH, parent), proto_(proto), algo_(algo), rdm_(rdm), replay_(0) {
    AuthInfoLen_ = getDigestSize(parent->DigestType);
}

void TOptAuthentication::setRDM(AuthReplay value) {
    rdm_ = value;
}

size_t TOptAuthentication::getSize() {
    return 17 + AuthInfoLen_;
}

bool TOpt::doDuties() {
    return true;
}

char * TOptAuthentication::storeSelf( char* buf) {
    AuthInfoLen_ = getDigestSize(Parent->DigestType);
    uint32_t spi = Parent->getSPI();
    uint32_t aaaspi = Parent->getAAASPI();

    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    *buf = rdm_;
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

void TOptAuthentication::setReplayDetection(uint64_t value) {
    replay_ = value;
}

uint64_t TOptAuthentication::getReplayDetection() {
    return replay_;
}

bool TOptAuthentication::doDuties() {
    return true;
}
