/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptTA.cpp,v 1.2 2006-03-05 21:37:46 thomson Exp $
 */

#include "Portable.h"
#include "OptTA.h"
#include "OptIAAddress.h"
#include "OptStatusCode.h"
#include "Logger.h"

TOptTA::TOptTA(uint32_t iaid, TMsg* parent)
    :TOpt(OPTION_IA_TA, parent), IAID_(iaid), Valid_(true) {
}

unsigned long TOptTA::getIAID() {
    return IAID_;
}

TOptTA::TOptTA(char * &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_IA_TA, parent), Valid_(false) {
    if (bufsize < OPTION_IA_TA_LEN) {
        buf += bufsize;
        bufsize = 0;
        return;
    }

    IAID_ = readUint32(buf);
    buf += sizeof(uint32_t);
    bufsize -= sizeof(uint32_t);
    Valid_ = true;

    /// @todo: Parse suboptions
}

int TOptTA::getStatusCode() {
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

size_t TOptTA::getSize() {
    return 4 + OPTION_IA_TA_LEN + getSubOptSize();
}

char * TOptTA::storeSelf( char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    buf = writeUint32(buf, IAID_);
    buf = storeSubOpt(buf);
    return buf;
}

unsigned long TOptTA::getMaxValid() {
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

bool TOptTA::isValid() const {
    return this->Valid;
}

/*
 * How many addresses is stored in this IA
 */
int TOptTA::countAddrs() {
    int cnt = 0;
    SPtr<TOpt> opt;
    this->firstOption();
    while (opt = this->getOption() ) {
        if (opt->getOptType() == OPTION_IAADDR)
            cnt++;
    }
    return cnt;
}
