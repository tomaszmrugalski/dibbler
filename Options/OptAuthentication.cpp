/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 licence
 */

#include <string.h>
#include <stdlib.h>
#include "Portable.h"
#include "OptAuthentication.h"
#include "DHCPConst.h"
#include "Portable.h"
#include "Portable.h"
#include "Logger.h"
#include "Msg.h"
#include "Portable.h"

TOptAuthentication::TOptAuthentication(char* buf, size_t buflen, TMsg* parent)
    :TOpt(OPTION_AUTH, parent), authDataPtr_(NULL) {

    if (buflen < OPT_AUTH_FIXED_SIZE) {
        Valid = false;
        return;
    }

    uint8_t proto = buf[0];
    switch (proto) {
    case AUTH_PROTO_NONE:
    case AUTH_PROTO_DELAYED:
    case AUTH_PROTO_RECONFIGURE_KEY:
    case AUTH_PROTO_DIBBLER:
        proto_ = static_cast<AuthProtocols>(proto);
        break;
    default: {
        Log(Warning) << "Auth: unsupported protocol type " << proto
                     << " received in auth option." << LogEnd;
        Valid = false;
        return;
    }
    }

    algo_ = buf[1];

    uint8_t rdm = buf[2];
    if (rdm <= AUTH_REPLAY_MONOTONIC) {
        rdm_ = static_cast<AuthReplay>(rdm);
    } else {
        Log(Warning) << "Auth: unsupported rdm value " << rdm
                     << " received in auth option." << LogEnd;
        Valid = false;
        return;
    }

    replay_ = readUint64(buf);

    buf += OPT_AUTH_FIXED_SIZE;
    buflen -= OPT_AUTH_FIXED_SIZE;

    switch (proto_) {
    default:
    case AUTH_PROTO_NONE:
    case AUTH_PROTO_DELAYED:
    case AUTH_PROTO_RECONFIGURE_KEY: {
        data_ = std::vector<uint8_t>(buf, buf + buflen);
        break;
    }

    case AUTH_PROTO_DIBBLER: {
        Parent->setSPI(readUint32(buf));
        buf += sizeof(uint32_t);
        buflen -= sizeof(uint32_t);

        AuthInfoLen_ = getDigestSize(parent->DigestType_);
        if (buflen != AuthInfoLen_){
            Valid = false;
            return;
        }

        Parent->setAuthDigestPtr(buf, AuthInfoLen_);
        PrintHex("Auth: Received digest: ", (uint8_t*)buf, AuthInfoLen_);
    }
    }

    Valid = true;
}

TOptAuthentication::TOptAuthentication(AuthProtocols proto, uint8_t algo,
                                       AuthReplay rdm, TMsg* parent)
    :TOpt(OPTION_AUTH, parent), proto_(proto), algo_(algo), rdm_(rdm), replay_(0),
    authDataPtr_(NULL) {
    if (parent) {
        AuthInfoLen_ = getDigestSize(parent->DigestType_);
    } else {
        AuthInfoLen_ = 0;
    }
}

void TOptAuthentication::setRDM(AuthReplay value) {
    rdm_ = value;
}

size_t TOptAuthentication::getSize() {
    size_t tmp = TOpt::OPTION6_HDR_LEN + OPT_AUTH_FIXED_SIZE + AuthInfoLen_;
    switch (proto_) {
    default:
        return tmp;
    case AUTH_PROTO_DIBBLER:
        return tmp + sizeof(uint32_t); // +4 bytes for SPI
    }
}

bool TOpt::doDuties() {
    return true;
}

char* TOptAuthentication::storeSelf(char* buf) {

    // common part (the same for all auth protocols)
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4);
    buf = writeUint8(buf, static_cast<char>(proto_));
    buf = writeUint8(buf, algo_);
    buf = writeUint8(buf, static_cast<char>(rdm_));
    buf = writeUint64(buf, getReplayDetection());

    switch (proto_) {

    default:
    case AUTH_PROTO_NONE:
    case AUTH_PROTO_DELAYED:
    case AUTH_PROTO_RECONFIGURE_KEY: {
        authDataPtr_ = buf;
        buf = writeData(buf, (char*)&data_[0], data_.size());
        return buf;
    }

    case AUTH_PROTO_DIBBLER: {
        AuthInfoLen_ = getDigestSize(Parent->DigestType_);

        // write SPI first
        buf = writeUint32(buf, Parent->getSPI());

        // Reserve space for the digest
        Parent->setAuthDigestPtr(buf, AuthInfoLen_);
        memset(buf, 0, AuthInfoLen_);
        buf += AuthInfoLen_;
    }
    }

    return buf;
}

AuthProtocols TOptAuthentication::getProto() const {
    return proto_;
}

void TOptAuthentication::setReplayDetection(uint64_t value) {
    replay_ = value;
}

uint64_t TOptAuthentication::getReplayDetection() {
    return replay_;
}

void TOptAuthentication::setPayload(const std::vector<uint8_t>& data) {
    data_ = data;
}

bool TOptAuthentication::doDuties() {
    return true;
}
