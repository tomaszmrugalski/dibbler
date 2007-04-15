/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptAddrParams.h,v 1.1.2.1 2007-04-15 21:23:31 thomson Exp $
 *
 */

#ifndef CLNTOPTADDRPARAMS_H
#define CLNTOPTADDRPARAMS_H

#include "OptInteger.h"

class TClntOptAddrParams : public TOptInteger
{
public:
    TClntOptAddrParams(char * buf, int n, TMsg* parent);

    int getPrefix();
    int getBitfield();
    bool doDuties();
};

#endif
