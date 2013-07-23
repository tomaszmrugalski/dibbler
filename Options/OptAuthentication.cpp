/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomek Mrugalski <thomson@klub.com.pl>
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
    :TOpt(OPTION_AUTH, parent), authDataPtr_(NULL), AuthInfoLen_(0) {

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
        data_ = std::vector<uint8_t>(buf, buf + buflen);
        if (Parent)
            Parent->setAuthDigestPtr(buf, buflen);
        break;
    case AUTH_PROTO_DELAYED: {
        // We can have 2 cases here:
        // 1. this is SOLICIT and client sends no data, buflen should be zero
        // 2. this is something else, so the data format should follow section 21.4.1
        //    i.e. have 4bytes key ID, 128bis of HMAC-MD5 and the rest is 'realm'
        if (buflen == 0) {
            // SOLICIT option, without any data.
            break;
        }
        if (buflen < DELAYED_AUTH_DIGEST_SIZE + DELAYED_AUTH_KEY_ID_SIZE) {
            Log(Warning) << "AUTH: protocol set to delayed-auth, but variable option data"
                         << " is smaller (" << buflen << ") than required 20. Invalid auth."
                         << LogEnd;
            Valid = false;
            return;
        }

        // The gentleman who designed delayed auth protocol was not exactly sober
        // during the act of creation.
        data_ = std::vector<uint8_t>(buf, buf + buflen); // let's store that crap in data_

        // First undetermined number of bytes is DHCP realm
        // (fine idea to start with variable field. Why not?)
        int realm_size = buflen - DELAYED_AUTH_DIGEST_SIZE - DELAYED_AUTH_KEY_ID_SIZE;
        realm_ = std::string(buf, buf + realm_size);
        buf += realm_size;
        buflen -= realm_size;

        // The next 4 bytes (RFC3315 never says that it's exactly 4 bytes. Nah, every
        // implementor will just guess the same size)
        if (Parent)
            Parent->setSPI(readUint32(buf));
        buf += sizeof(uint32_t);
        buflen -= sizeof(uint32_t);

        // The rest should be 16 bytes of of HMAC-MD5 data
        if (Parent)
            Parent->setAuthDigestPtr(buf, buflen); // that should be the last 16 bytes.
        break;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: {
        data_ = std::vector<uint8_t>(buf, buf + buflen);
        if (Parent)
            Parent->setAuthDigestPtr(buf + 1, buflen);
        if (data_.size() != RECONFIGURE_KEY_AUTHINFO_SIZE) {
            Log(Warning) << "AUTH: Invalid reconfigure-key data received. Expected size is "
                         << RECONFIGURE_KEY_AUTHINFO_SIZE << ", but received " << data_.size()
                         << LogEnd;
            Valid = false;
            return;
        }
        break;
    }

    case AUTH_PROTO_DIBBLER: {
        if (algo_ >= DIGEST_INVALID) {
            Log(Warning) << "Unsupported digest type: " << algo_ << ", max supported "
                         << " type is " << DIGEST_INVALID - 1 << LogEnd;
            Valid = false;
            return;
        }
        if (!Parent) {
            Log(Error) << "Orphaned option AUTH: no parent set." << LogEnd;
            Valid = false;
            return;
        }
        Parent->DigestType_ = static_cast<DigestTypes>(algo_);
        Parent->setSPI(readUint32(buf));
        buf += sizeof(uint32_t);
        buflen -= sizeof(uint32_t);

        data_ = std::vector<uint8_t>(buf, buf + buflen);

        AuthInfoLen_ = getDigestSize(parent->DigestType_);
        if (buflen != AuthInfoLen_){
            Log(Warning) << "Auth: Invalid digest size for digest type " << algo_
                       << ", expected len=" << AuthInfoLen_ << ", received " << buflen << LogEnd;
            Valid = false;
            return;
        }

        Parent->setAuthDigestPtr(buf, AuthInfoLen_);
        PrintHex(std::string("Auth: Received digest ") + getDigestName(parent->DigestType_) + ": ",
                 (uint8_t*)buf, AuthInfoLen_);
    }
    }

    Valid = true;
}

uint8_t TOptAuthentication::getAlgorithm() const {
    return algo_;
}

AuthReplay TOptAuthentication::getRDM() const {
    return rdm_;
}

TOptAuthentication::TOptAuthentication(AuthProtocols proto, uint8_t algo,
                                       AuthReplay rdm, TMsg* parent)
    :TOpt(OPTION_AUTH, parent), proto_(proto), algo_(algo), rdm_(rdm), replay_(0),
    authDataPtr_(NULL) {
    switch (proto) {
    default:
        AuthInfoLen_ = 0;
        return;
    case AUTH_PROTO_DELAYED:
        if (algo != 1) {
            Log(Warning) << "AUTH: The only defined protocol for delayed-auth is 1, but "
                         << static_cast<int>(algo) << " was specified." << LogEnd;
        }
        AuthInfoLen_ = 0; // Keep it as zero (we don't know yet if it's a SOLICIT
                          // (no data at all) or any other (realm + key id + HMAC-MD5)
        return;
    case AUTH_PROTO_DIBBLER: {
        if (parent) {
            AuthInfoLen_ = getDigestSize(parent->DigestType_);
        } else {
            AuthInfoLen_ = 0;
        }
        return;
    }
    case AUTH_PROTO_RECONFIGURE_KEY:
        AuthInfoLen_ = RECONFIGURE_DIGEST_SIZE; // Sec section 21.5.1, RFC3315
        return;
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
    case AUTH_PROTO_DELAYED:
        if (realm_.empty()) {
            return tmp;
        } else {
            return tmp + realm_.size() + DELAYED_AUTH_KEY_ID_SIZE;
        }
    case AUTH_PROTO_RECONFIGURE_KEY:
        return tmp + 1;
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

    case AUTH_PROTO_RECONFIGURE_KEY: {
        if (!data_.size()) {
            return buf;
        }

        switch (data_[0]) {
        default:
        case 1: // reconfigure-key value
            authDataPtr_ = buf;
            buf = writeData(buf, (char*)&data_[0], data_.size());
            return buf;
        case 2: // HMAC-MD5 digest
            authDataPtr_ = buf + 1; // The first byte is 1 or 2 (see RFC3315, 21.5.1)
            buf[0] = data_[0]; // 2 = HMAC-MD5 digest
            memset(buf + 1, 0, RECONFIGURE_DIGEST_SIZE); // 16 bytes
            return buf + RECONFIGURE_KEY_AUTHINFO_SIZE;
        }
    }

    default:
    case AUTH_PROTO_NONE: {
        authDataPtr_ = buf;
        buf = writeData(buf, (char*)&data_[0], data_.size());
        return buf;
    }
    case AUTH_PROTO_DELAYED: {
        if (realm_.empty()) {
            // SOLICIT without any data
            return buf;
        } else {
            // Realm set, build the option for real

            memcpy(buf, &realm_[0], realm_.size()); // realm (variable)
            buf += realm_.size();
            if (Parent)
                buf = writeUint32(buf, Parent->getSPI()); // key id (4 bytes)
            else
                buf = writeUint32(buf, 0);
            authDataPtr_ = buf; // Digest will be stored here
            memset(buf, 0, AuthInfoLen_); // HMAC-MD5 (16 bytes)
            buf += AuthInfoLen_;
            return buf;
        }
    }

    case AUTH_PROTO_DIBBLER: {
        if (!Parent) {
            return writeUint32(buf, 0); // no SPI at all
        }

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

void TOptAuthentication::getPayload(std::vector<uint8_t>& data) {
    data = data_;
}

bool TOptAuthentication::doDuties() {
    return true;
}

void TOptAuthentication::setRealm(const std::string& realm) {
    realm_ = realm;
    if (realm_.empty()) {
        AuthInfoLen_ = 0;
    } else {
        AuthInfoLen_ = DELAYED_AUTH_DIGEST_SIZE;
    }
}
