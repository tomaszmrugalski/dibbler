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

TOptIA_NA::TOptIA_NA( long IAID, long t1,  long t2, TMsg* parent)
	:TOpt(OPTION_IA_NA, parent) {
    this->IAID = IAID;
    this->T1   = t1;
    this->T2   = t2;
}

unsigned long TOptIA_NA::getIAID() {
    return IAID;
}

unsigned long TOptIA_NA::getT1() {
    return T1;
}

void TOptIA_NA::setT1(unsigned long t1) {
    this->T1 = t1;
}

unsigned long TOptIA_NA::getT2() {
    return T2;
}

void TOptIA_NA::setT2(unsigned long t2) {
    this->T2 = t2;
}

TOptIA_NA::TOptIA_NA( char * &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_IA_NA, parent) {
    if (bufsize<12) {
        Valid=false;
        bufsize=0;
    } else {
        Valid=true;
        IAID = readUint32(buf);
        buf += sizeof(uint32_t); bufsize -= sizeof(uint32_t);
        T1 = readUint32(buf);
        buf += sizeof(uint32_t); bufsize -= sizeof(uint32_t);
        T2 = readUint32(buf);
        buf += sizeof(uint32_t); bufsize -= sizeof(uint32_t);
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

int TOptIA_NA::getSize() {
    int mySize = 16;
    return mySize+getSubOptSize();
}

char * TOptIA_NA::storeSelf( char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4 );

    buf = writeUint32(buf, IAID);
    buf = writeUint32(buf, T1);
    buf = writeUint32(buf, T2);

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

bool TOptIA_NA::isValid() {
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
