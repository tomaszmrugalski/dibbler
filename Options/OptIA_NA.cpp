/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include "Portable.h"
#include "OptIA_NA.h"
#include "OptIAAddress.h"
#include "OptStatusCode.h"
//#include "OptRapidCommit.h"

TOptIA_NA::TOptIA_NA(long IAID, long t1,  long t2, TMsg* parent)
    :TOpt(OPTION_IA_NA, parent), IAID_(IAID), T1_(t1), T2_(t2), Valid_(true) {
}

void TOptIA_NA::setIAID(uint32_t iaid) {
    IAID_ = iaid;
}

unsigned long TOptIA_NA::getIAID() const {
    return IAID_;
}

unsigned long TOptIA_NA::getT1() const {
    return T1_;
}

void TOptIA_NA::setT1(unsigned long t1) {
    T1_ = t1;
}

unsigned long TOptIA_NA::getT2() const {
    return T2_;
}

void TOptIA_NA::setT2(unsigned long t2) {
    T2_ = t2;
}

TOptIA_NA::TOptIA_NA( char * &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_IA_NA, parent), Valid_(false) {
    if (bufsize < 12) {
        buf += bufsize;
        bufsize = 0;
    } else {
        IAID_ = readUint32(buf);
        buf += sizeof(uint32_t);
        bufsize -= sizeof(uint32_t);

        T1_ = readUint32(buf);
        buf += sizeof(uint32_t);
        bufsize -= sizeof(uint32_t);

        T2_ = readUint32(buf);
        buf += sizeof(uint32_t); bufsize -= sizeof(uint32_t);
        Valid_ = true;
    }
}


int TOptIA_NA::getStatusCode() {
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

size_t TOptIA_NA::getSize() {
    int mySize = 16;
    return mySize + getSubOptSize();
}

char * TOptIA_NA::storeSelf( char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4 );

    buf = writeUint32(buf, IAID_);
    buf = writeUint32(buf, T1_);
    buf = writeUint32(buf, T2_);

    buf = storeSubOpt(buf);
    return buf;
}

unsigned long TOptIA_NA::getMaxValid() {
    unsigned long maxValid=0;
    SPtr<TOpt> ptrOpt;
    SubOptions.first();
    while (ptrOpt=SubOptions.get())
    {
        if (ptrOpt->getOptType()==OPTION_IAADDR) {
            SPtr<TOptIAAddress> ptrIAAddr=(Ptr*)ptrOpt;
            if (maxValid<ptrIAAddr->getValid())
                maxValid=ptrIAAddr->getValid();
        }
    }
    return maxValid;
}

bool TOptIA_NA::isValid() const {
    return this->Valid;
}

/*
 * How many addresses is stored in this IA
 */
int TOptIA_NA::countAddrs() {
    int cnt = 0;
    SPtr<TOpt> opt;
    this->firstOption();
    while (opt = this->getOption() ) {
        if (opt->getOptType() == OPTION_IAADDR)
            cnt++;
    }
    return cnt;
}
