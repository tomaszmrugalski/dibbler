/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptLifetime.h,v 1.1 2004-10-25 20:45:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTOPTLIFETIME_H
#define CLNTOPTLIFETIME_H

#include "DHCPConst.h"
#include "OptInteger4.h"

class TClntOptLifetime : public TOptInteger4
{
 public:
    TClntOptLifetime(char * buf,  int n, TMsg* parent);
    
    TClntOptLifetime(char pref, TMsg* parent);
    bool doDuties();
};

#endif
