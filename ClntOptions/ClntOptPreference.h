/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptPreference.h,v 1.2 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTPREFERENCE_H
#define CLNTPREFERENCE_H

#include "DHCPConst.h"
#include "OptPreference.h"

class TClntOptPreference : public TOptPreference 
{
  public:
    TClntOptPreference( char * buf,  int n, TMsg* parent);

    TClntOptPreference( char pref, TMsg* parent);
	bool doDuties();
};

#endif
