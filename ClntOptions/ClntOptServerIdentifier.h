/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptServerIdentifier.h,v 1.2 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTSERVERIDENTIFIER_H
#define CLNTSERVERIDENTIFIER_H

#include "DHCPConst.h"
#include "OptServerIdentifier.h"

class TClntOptServerIdentifier : public TOptServerIdentifier 
{
  public:
    TClntOptServerIdentifier( char * duid, int n, TMsg* parent);
    TClntOptServerIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
	bool doDuties();
};
#endif
