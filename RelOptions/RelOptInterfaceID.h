/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelOptInterfaceID.h,v 1.4 2008-08-29 00:07:32 thomson Exp $
 *
 */

#ifndef RELOPTIONINTERFACEID_H
#define RELOPTIONINTERFACEID_H
#include "OptInteger.h"

class TRelOptInterfaceID : public TOptInteger
{
  public:
    TRelOptInterfaceID(char * data, int dataLen, TMsg* parent);
    TRelOptInterfaceID(int interfaceID, TMsg* parent);
    bool doDuties();
};

#endif /* RELOPTIONINTERFACEID_H */
