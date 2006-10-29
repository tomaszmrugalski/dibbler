/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptLifetime.h,v 1.2 2006-10-29 13:11:46 thomson Exp $
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
