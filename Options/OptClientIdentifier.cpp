#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "DHCPConst.h"
#include "OptClientIdentifier.h"

 int TOptClientIdentifier::getSize()
{
    return this->DUID->getLen() + 4;
}

TOptClientIdentifier::TOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent)
	:TOpt(OPTION_CLIENTID, parent)
{
	//DUID = new  char[n];
	//memcpy(DUID,duid,n);
	//DUIDlen = n;
    this->DUID=duid;
}

 char * TOptClientIdentifier::storeSelf( char* buf)
{
    *(uint16_t*)buf = htons(OptType);
    buf+=2;
    *(uint16_t*)buf = htons(DUID->getLen());
    buf+=2;
    return this->DUID->storeSelf(buf);
}

TOptClientIdentifier::TOptClientIdentifier(char* &buf, int &bufsize, TMsg* parent)
    :TOpt(OPTION_CLIENTID, parent)
{
    this->DUID=new TDUID(buf,bufsize);
    buf+=DUID->getLen(); 
    bufsize-=DUID->getLen();
}

/* int TOptClientIdentifier::getDUIDlen()
{
	return DUIDlen;
}*/

SmartPtr<TDUID> TOptClientIdentifier::getDUID()
{
	return DUID;
}

bool TOptClientIdentifier::isValid()
{
    if (this->getDUID()->getLen()>2) 
        return true;
    return false;
}