/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTOPTIAPREFIX_H
#define CLNTOPTIAPREFIX_H

#include "SmartPtr.h"
#include "Container.h"
#include "OptIAPrefix.h"

class TClntOptIAPrefix : public TOptIAPrefix
{
 public:
    TClntOptIAPrefix(char *addr,int n, TMsg* parent);
    TClntOptIAPrefix(SPtr<TIPv6Addr> addr, long pref, long valid, char prefix_length, TMsg* parent);
    bool doDuties();
    bool isValid();
};

#endif /* CLNTOPTIAPREFIX_H */
