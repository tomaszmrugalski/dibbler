/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptClientIdentifier.cpp,v 1.5 2006-01-29 10:48:31 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/07/17 19:56:54  thomson
 * End-of-line problem solved.
 *
 * Revision 1.3  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include "DHCPConst.h"
#include "ClntOptClientIdentifier.h"


TClntOptClientIdentifier::TClntOptClientIdentifier(char * duid, int n, TMsg* parent)
    :TOptDUID(duid,n, parent)
{
}

TClntOptClientIdentifier::TClntOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent)
    :TOptDUID(duid, parent)
{
}

bool TClntOptClientIdentifier::doDuties()
{
    return false;
}
