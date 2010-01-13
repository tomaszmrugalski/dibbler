/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef SRVOPTSERVERIDENTIFIER_H
#define SRVOPTSERVERIDENTIFIER_H

#include "DHCPConst.h"
#include "OptDUID.h"

class TSrvOptServerIdentifier : public TOptDUID
{
  public:
    TSrvOptServerIdentifier( SPtr<TDUID> duid, TMsg* parent);
    TSrvOptServerIdentifier(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif 
