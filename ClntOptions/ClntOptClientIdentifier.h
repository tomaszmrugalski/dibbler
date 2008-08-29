/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTOPTCLIENTIDENTIFIER_H
#define CLNTOPTCLIENTIDENTIFIER_H

#include "OptDUID.h"

class TClntOptClientIdentifier : public TOptDUID
{
  public:
    TClntOptClientIdentifier(char * duid,int n, TMsg* parent);
    TClntOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
    bool doDuties();
};

#endif /* CLNTOPTCLIENTTIDENTIFIER_H */
