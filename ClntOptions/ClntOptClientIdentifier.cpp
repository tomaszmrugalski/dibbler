/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */
#include "DHCPConst.h"
#include "ClntOptClientIdentifier.h"


TClntOptClientIdentifier::TClntOptClientIdentifier(char * duid, int n, TMsg* parent)
    :TOptDUID(OPTION_CLIENTID, duid,n, parent)
{
}

TClntOptClientIdentifier::TClntOptClientIdentifier(SPtr<TDUID> duid, TMsg* parent)
    :TOptDUID(OPTION_CLIENTID, duid, parent)
{
}

bool TClntOptClientIdentifier::doDuties()
{
    return false;
}
