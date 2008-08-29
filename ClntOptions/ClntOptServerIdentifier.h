/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptServerIdentifier.h,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2006-01-29 10:48:31  thomson
 * Base class changed.
 *
 * Revision 1.2  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#ifndef CLNTSERVERIDENTIFIER_H
#define CLNTSERVERIDENTIFIER_H

#include "DHCPConst.h"
#include "OptDUID.h"

class TClntOptServerIdentifier : public TOptDUID
{
  public:
    TClntOptServerIdentifier( char * duid, int n, TMsg* parent);
    TClntOptServerIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
	bool doDuties();
};
#endif
