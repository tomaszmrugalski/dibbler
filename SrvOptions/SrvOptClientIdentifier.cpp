/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */


#include <stdlib.h>
#include "Portable.h"
#include "DHCPConst.h"
#include "SrvOptClientIdentifier.h"

TSrvOptClientIdentifier::TSrvOptClientIdentifier(
    SPtr<TDUID> duid, TMsg* parent)
    :TOptDUID(OPTION_CLIENTID, duid, parent)
{
}

TSrvOptClientIdentifier::TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent)
    :TOptDUID(OPTION_CLIENTID, buf,bufsize, parent)
{
}

bool TSrvOptClientIdentifier::doDuties()
{
    return true;
}
