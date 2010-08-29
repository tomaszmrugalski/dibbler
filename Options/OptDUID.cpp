/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "OptDUID.h"

TOptDUID::TOptDUID(int type, SPtr<TDUID> duid, TMsg* parent)
    :TOpt(type, parent)
{
    DUID=duid;
}

 int TOptDUID::getSize()
{
    if (this->DUID)
	return this->DUID->getLen()+4;
    return 4;
}

 char * TOptDUID::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(DUID->getLen());
    buf+=2;
    return this->DUID->storeSelf(buf);
}

TOptDUID::TOptDUID(int type, const char* buf, int bufsize, TMsg* parent)
    :TOpt(type, parent)
{
    this->DUID = new TDUID(buf,bufsize);
    // buf+=DUID->getLen(); 
    // bufsize-=DUID->getLen();
}

SPtr<TDUID> TOptDUID::getDUID()
{
	return DUID;
}

bool TOptDUID::isValid()
{
    if (this->getDUID()->getLen()>2) 
        return true;
    return false;
}
