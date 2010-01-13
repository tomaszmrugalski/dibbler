/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTOPTIAADDRESS_H
#define CLNTOPTIAADDRESS_H

#include "SmartPtr.h"
#include "Container.h"
#include "OptIAAddress.h"

class TClntOptIAAddress : public TOptIAAddress
{
 public:
    TClntOptIAAddress(char *addr,int n, TMsg* parent);
    TClntOptIAAddress(SPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent);
    bool doDuties();
    bool isValid();
};

#endif /* CLNTOPTIAADDRESS_H */
