/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include <stdlib.h>

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "OptServerIdentifier.h"

TOptServerIdentifier::TOptServerIdentifier( SmartPtr<TDUID> duid, TMsg* parent)
    :TOpt(OPTION_SERVERID, parent)
{
	/*DUID = new  char[n];
	memcpy(DUID,duid,n);
	DUIDlen = n;*/
    this->DUID=duid;
}

 int TOptServerIdentifier::getSize()
{
    return this->DUID->getLen()+4;
}

 char * TOptServerIdentifier::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(DUID->getLen());
    buf+=2;
    return this->DUID->storeSelf(buf);
}

/* int TOptServerIdentifier::getDUIDlen()
{
	return DUIDlen;
}*/

TOptServerIdentifier::TOptServerIdentifier(char* &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_SERVERID, parent)
{
    this->DUID=new TDUID(buf,bufsize);
    buf+=DUID->getLen(); 
    bufsize-=DUID->getLen();
}

SmartPtr<TDUID> TOptServerIdentifier::getDUID()
{
	return DUID;
}

bool TOptServerIdentifier::isValid()
{
    if (this->getDUID()->getLen()>2) 
        return true;
    return false;
}
