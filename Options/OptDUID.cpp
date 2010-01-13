/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptDUID.cpp,v 1.2 2006-03-05 21:37:46 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1.2.1  2006/02/05 23:38:08  thomson
 * Devel branch with Temporary addresses support added.
 *
 * Revision 1.1  2006/01/29 10:46:46  thomson
 * Opt{Client|Server}Identifier classes removed, OptDUID added.
 *
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
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
    this->DUID=duid;
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

TOptDUID::TOptDUID(int type, char* &buf, int &bufsize, TMsg* parent)
    :TOpt(type, parent)
{
    this->DUID=new TDUID(buf,bufsize);
    buf+=DUID->getLen(); 
    bufsize-=DUID->getLen();
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
