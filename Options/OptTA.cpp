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

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "OptTA.h"
#include "OptIAAddress.h"
#include "OptStatusCode.h"
#include "Logger.h"

TOptTA::TOptTA( long IAID, TMsg* parent)
	:TOpt(OPTION_IA_TA, parent) {
    this->IAID = IAID;
}

unsigned long TOptTA::getIAID() {
    return IAID;
}

TOptTA::TOptTA( char * &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_IA_TA, parent) {
    if (bufsize<OPTION_IA_TA_LEN) {
        Valid=false;
        bufsize=0;
    } else {
        Valid=true;
        this->IAID = ntohl(*( long*)buf);
        buf+=4; bufsize-=4;
    }
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

int TOptTA::getSize() {
    return 4+OPTION_IA_TA_LEN+getSubOptSize();
}

char * TOptTA::storeSelf( char* buf) {
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons( getSize()-4 );
    buf+=2;
    
    *(uint32_t*)buf = htonl(IAID);
    buf+=4;
    buf=this->storeSubOpt(buf);
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

bool TOptTA::isValid() {
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
/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.2.1  2006/02/05 23:39:52  thomson
 * Initial revision.
 *
 */
