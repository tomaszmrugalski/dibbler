/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 * 
 * released under GNU GPL v2 only licence
 *
 *
 */

#include "Portable.h"
#include "OptIA_PD.h"
#include "OptIAPrefix.h"
#include "OptStatusCode.h"


TOptIA_PD::TOptIA_PD( long IAID, long t1,  long t2, TMsg* parent)
	:TOpt(OPTION_IA_PD, parent) {
    this->IAID = IAID;
    this->T1   = t1;
    this->T2   = t2;
}

unsigned long TOptIA_PD::getIAID() {
    return IAID;
}

unsigned long TOptIA_PD::getT1() {
    return T1;
}

unsigned long TOptIA_PD::getT2() {
    return T2;
}

TOptIA_PD::TOptIA_PD( char * &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_IA_PD, parent) {
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

int TOptIA_PD::getSize() {
    int mySize = 16;
    return mySize+getSubOptSize();
}

char * TOptIA_PD::storeSelf( char* buf) {
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize() - 4 );

    buf = writeUint32(buf, IAID);
    buf = writeUint32(buf, T1);
    buf = writeUint32(buf, T2);

    buf = storeSubOpt(buf);
    return buf;
}

unsigned long TOptIA_PD::getMaxValid() {
    unsigned long maxValid=0;
    SPtr<TOpt> ptrOpt;
    SubOptions.first();
    while (ptrOpt=SubOptions.get())
    {
        if (ptrOpt->getOptType()==OPTION_IAPREFIX) {
            //SPtr<TOptIAAddress> ptrIAAddr=(Ptr*)ptrOpt;
            //if (maxValid<ptrIAAddr->getValid())
            //    maxValid=ptrIAAddr->getValid();
        }
    }   
    return maxValid;
}

bool TOptIA_PD::isValid() {
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
