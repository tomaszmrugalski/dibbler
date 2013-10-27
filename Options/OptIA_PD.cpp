/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "Portable.h"
#include "OptIA_PD.h"
#include "OptIAPrefix.h"
#include "OptStatusCode.h"


TOptIA_PD::TOptIA_PD(uint32_t iaid, uint32_t t1, uint32_t t2, TMsg* parent)
    :TOpt(OPTION_IA_PD, parent), IAID_(iaid), T1_(t1), T2_(t2), Valid_(true)  {
}

uint32_t TOptIA_PD::getIAID() const {
    return IAID_;
}

uint32_t TOptIA_PD::getT1() const {
    return T1_;
}

uint32_t TOptIA_PD::getT2() const {
    return T2_;
}

TOptIA_PD::TOptIA_PD(char * &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_IA_PD, parent), Valid_(false) {
    if (bufsize < 12) {
        bufsize = 0;
    } else {
        IAID_ = readUint32(buf);
        buf += sizeof(uint32_t);
        bufsize -= sizeof(uint32_t);

        T1_ = readUint32(buf);
        buf += sizeof(uint32_t);
        bufsize -= sizeof(uint32_t);

        T2_ = readUint32(buf);
        buf += sizeof(uint32_t);
        bufsize -= sizeof(uint32_t);
        Valid = true;
    }
}


int TOptIA_PD::getStatusCode() {
    SPtr<TOpt> ptrOpt;
    SubOptions.first();
    while ( ptrOpt = SubOptions.get() ) {
        if ( ptrOpt->getOptType() == OPTION_STATUS_CODE) {
            SPtr <TOptStatusCode> ptrStatus;
            ptrStatus = (Ptr*) ptrOpt;
            return ptrStatus->getCode();
        }
    }
    return -1;
}

size_t TOptIA_PD::getSize() {
    int mySize = 16;
    return mySize + getSubOptSize();
}

char * TOptIA_PD::storeSelf( char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4 );

    buf = writeUint32(buf, IAID_);
    buf = writeUint32(buf, T1_);
    buf = writeUint32(buf, T2_);

    buf = storeSubOpt(buf);
    return buf;
}

bool TOptIA_PD::isValid() const {
    /// @todo check if suboptions are valid
    return this->Valid;
}

/*
 * How many prefixes are stored in this IA_PD
 */
int TOptIA_PD::countPrefixes() {
    int cnt = 0;
    SPtr<TOpt> opt;
    this->firstOption();
    while (opt = this->getOption() ) {
        if (opt->getOptType() == OPTION_IAPREFIX)
            cnt++;
    }
    return cnt;
}

void TOptIA_PD::setT1(uint32_t t1) {
    T1_ = t1;
}

void TOptIA_PD::setT2(uint32_t t2) {
    T2_ = t2;
}

void TOptIA_PD::setIAID(uint32_t iaid) {
    IAID_ = iaid;
}
