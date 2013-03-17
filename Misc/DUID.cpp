/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <iostream>
#include <sstream>
#include <string.h>

#include "DUID.h"
#include "Logger.h"

using namespace std;

TDUID::TDUID()
    :DUID_(NULL), Len_(0), Plain_("")
{
}

// packed
TDUID::TDUID(const char* duid,int len)
{
    if (duid && len) {
        DUID_ = new char[len];
        Len_ = len;
        memcpy(DUID_, duid, len);
        packedToPlain();
    } else {
        DUID_ = NULL;
        Len_ = 0;
        Plain_ = "";
    }
}

// plain
TDUID::TDUID(const char* text)
{
    if (!text) {
        DUID_ = NULL;
        Len_ = 0;
        Plain_ = string("");
        return;
    }
    Plain_ = string(text);
    plainToPacked();
    packedToPlain();
}

void TDUID::packedToPlain() {
    ostringstream tmp;
    for(unsigned i = 0; i < Len_; i++) {
        if (i)
            tmp << ":";
        tmp << setfill('0') << setw(2) << hex
            << (unsigned short)((unsigned char) DUID_[i]);
    }
    tmp << dec;
    Plain_ = tmp.str();
}

void TDUID::plainToPacked() {
    int textLen = Plain_.length();
    char * tmp = new char[textLen>>1];
    unsigned char digit;
    int i=0, j=0;
    bool twonibbles = false;
    while (i < textLen)
    {
        if (Plain_[i]==':') {
            i++;
        }
        digit = Plain_[i];
        if (isalpha(digit))
            digit=toupper(digit)-'A'+10;
        else
            digit-='0';
        tmp[j]<<=4;
        tmp[j]|=digit;
        i++;
        if (twonibbles) {
            twonibbles = false;
            j++;
        } else
            twonibbles = true;
    }

    DUID_ = new char[j];
    memmove(DUID_, tmp, j);
    delete [] tmp;
    Len_ = j;
}

TDUID::~TDUID() {
    if (Len_ && DUID_)
        delete [] DUID_;
}

TDUID::TDUID(const TDUID &other) {
    DUID_ = new char[other.Len_];
    memcpy(DUID_, other.DUID_, other.Len_);
    Len_ = other.Len_;
    Plain_ = other.Plain_;
}

TDUID& TDUID::operator=(const TDUID &other) {
    if (this == &other)
        return *this;

    if(DUID_)
        delete DUID_;

    DUID_ = new char [other.Len_];
    memcpy(DUID_, other.DUID_, other.Len_);
    Len_ = other.Len_;
    Plain_ = other.Plain_;

    return *this;
}

bool TDUID::operator==(const TDUID &other) {
    if (Len_ != other.Len_)
        return false;
    else
        return !memcmp(DUID_, other.DUID_, Len_);
}

bool TDUID::operator<=(const TDUID &other) {

    size_t minLen = Len_;
    if (other.Len_ < minLen)
        minLen = other.Len_;

    //if they are not equal check if longer has only zeros at the begining
    bool eq = true;
    for (unsigned i=0; i<minLen; i++) {
        if (DUID_[i] == other.DUID_[i]) {
            continue;
        }
        eq = false;
        if (DUID_[i] > other.DUID_[i])
            return false;
    }

    // first minLen bytes are equal or this is smaller than the other
    if (!eq)
        return true;
    return (other.Len_ > Len_);
}

size_t TDUID::getLen() const {
    return Len_;
}

const string TDUID::getPlain() const {
    return Plain_;
}

const char* TDUID::get() const {
    return DUID_;
}

char * TDUID::storeSelf(char* buf) {
    memcpy(buf, DUID_, Len_);
    return buf + Len_;
}

ostream& operator<<(ostream& out,TDUID&  duid) {
    if ( (duid.DUID_ && duid.Len_) ) {
        out << "<duid length=\"" << duid.Len_ << "\">"
            << duid.Plain_ << "</duid>" << std::endl;
    } else {
        out << "<duid length=\"0\"></duid>" << std::endl;
    }
    return out;
}
