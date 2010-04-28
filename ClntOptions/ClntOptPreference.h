/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptPreference.h,v 1.3 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004-10-25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 *
 */

#ifndef CLNTPREFERENCE_H
#define CLNTPREFERENCE_H

#include "DHCPConst.h"
#include "OptInteger.h"

class TClntOptPreference : public TOptInteger
{
  public:
    TClntOptPreference( char * buf,  int n, TMsg* parent);

    TClntOptPreference( char pref, TMsg* parent);
	bool doDuties();
};

#endif
