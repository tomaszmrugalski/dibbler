/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptClientIdentifier.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
#include "ClntOptClientIdentifier.h"


TClntOptClientIdentifier::TClntOptClientIdentifier(char * duid, int n, TMsg* parent)
    :TOptClientIdentifier(duid,n, parent)
{
}

TClntOptClientIdentifier::TClntOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent)
    :TOptClientIdentifier(duid, parent)
{
}

bool TClntOptClientIdentifier::doDuties()
{
    return false;
}
