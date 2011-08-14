/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#if defined(LINUX) || defined(MACOS)
#include <netinet/in.h>
#endif 

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
        IAID = ntohl(*( long*)buf);
        T1 = ntohl(*( long*)(buf+4));
        T2 = ntohl(*( long*)(buf+8));
        buf+=12; bufsize-=12;
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
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    
    *(uint32_t*)buf = htonl(IAID);
    buf+=4;
    *(uint32_t*)buf = htonl(T1);
    buf+=4;
    *(uint32_t*)buf = htonl(T2);
    buf+=4;
    buf=this->storeSubOpt(buf);
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
