/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptLifetime.h,v 1.3 2008-08-29 00:07:29 thomson Exp $
 *
 */

#ifndef CLNTOPTLIFETIME_H
#define CLNTOPTLIFETIME_H

#include "DHCPConst.h"
#include "OptInteger.h"

class TClntOptLifetime : public TOptInteger
{
 public:
    TClntOptLifetime(char * buf,  int n, TMsg* parent);
    
    TClntOptLifetime(char pref, TMsg* parent);
    bool doDuties();
};

#endif
