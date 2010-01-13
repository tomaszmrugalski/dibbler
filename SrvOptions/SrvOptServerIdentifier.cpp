/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "SrvOptServerIdentifier.h"

TSrvOptServerIdentifier::TSrvOptServerIdentifier( SPtr<TDUID> duid, TMsg* parent)
    :TOptDUID(OPTION_SERVERID, duid, parent)
{
}

TSrvOptServerIdentifier::TSrvOptServerIdentifier(char* buf, int bufsize, TMsg* parent)
    :TOptDUID(OPTION_SERVERID, buf,bufsize, parent)
{
}

bool TSrvOptServerIdentifier::doDuties()
{
    return true;
}
