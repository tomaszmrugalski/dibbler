/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef SRVCLIENTIDENTIFIER_H
#define SRVCLIENTIDENTIFIER_H

#include "OptDUID.h"

class TOptDUID;

class TSrvOptClientIdentifier : public TOptDUID
{
 public:
    TSrvOptClientIdentifier( SPtr<TDUID> duid, TMsg* parent);
    TSrvOptClientIdentifier(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif
