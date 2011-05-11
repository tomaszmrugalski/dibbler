/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptAddrLst.cpp,v 1.1 2004-11-02 01:23:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 */


#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "OptAddrLst.h"
#include "DHCPConst.h"

TOptAddrLst::TOptAddrLst(int type, List(TIPv6Addr) lst, TMsg* parent)
    :TOpt(type, parent), AddrLst(lst) {
}

TOptAddrLst::TOptAddrLst(int type, const char* buf, unsigned short bufSize, TMsg* parent)
    :TOpt(type, parent)
{
    while(bufSize>0)
    {
	if (bufSize<16) {
	    Valid = false;
	    return;
	}
        this->AddrLst.append(new TIPv6Addr(buf));
	buf +=16;
	bufSize -= 16;
    }
    Valid = true;
    return;
}

char * TOptAddrLst::storeSelf(char* buf) {
    SPtr<TIPv6Addr> addr;

    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;
    AddrLst.first();
    while(addr=AddrLst.get())
	buf=addr->storeSelf(buf);
    return buf;
}

int TOptAddrLst::getSize()
{
    return 4+16*AddrLst.count();
}

void TOptAddrLst::firstAddr() {
    this->AddrLst.first();
}

SPtr<TIPv6Addr> TOptAddrLst::getAddr()
{
    return this->AddrLst.get();
}

bool TOptAddrLst::isValid()
{
    return this->Valid;
}

