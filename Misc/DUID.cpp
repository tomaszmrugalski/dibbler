/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <string.h>
#include "hex.h"
#include "DUID.h"
#include "Logger.h"

using namespace std;

TDUID::TDUID()
{
}

// packed
TDUID::TDUID(const char* duid,int len)
{
    if (duid && len) {
        DUID_.resize(len);
        memcpy(&DUID_[0], duid, len);
        Plain_ = hexToText((uint8_t*)&DUID_[0], DUID_.size(), true);
    }
}

// plain
TDUID::TDUID(const char* text)
{
    if (!text) {
        return;
    }
    Plain_ = string(text);
    DUID_ = textToHex(Plain_);
    Plain_ = hexToText((uint8_t*)&DUID_[0], DUID_.size(), true);
}


TDUID::~TDUID() {
}

TDUID::TDUID(const TDUID &other) {
    DUID_ = other.DUID_;
    Plain_ = other.Plain_;
}

TDUID& TDUID::operator=(const TDUID &other) {
    if (this == &other)
        return *this;

    DUID_ = other.DUID_;
    Plain_ = other.Plain_;

    return *this;
}

bool TDUID::operator==(const TDUID &other) {
    return (DUID_ == other.DUID_);
}

bool TDUID::operator<=(const TDUID &other) {

    return (DUID_ <= other.DUID_);
}

size_t TDUID::getLen() const {
    return DUID_.size();
}

const string TDUID::getPlain() const {
    return Plain_;
}

const char* TDUID::get() const {
    return (const char*)(&DUID_[0]);
}

char * TDUID::storeSelf(char* buf) {
    memcpy(buf, &DUID_[0], DUID_.size());
    return buf + DUID_.size();
}

ostream& operator<<(ostream& out,TDUID&  duid) {
    if (duid.DUID_.size()) {
        out << "<duid length=\"" << duid.DUID_.size() << "\">"
            << duid.Plain_ << "</duid>" << std::endl;
    } else {
        out << "<duid length=\"0\"></duid>" << std::endl;
    }
    return out;
}
