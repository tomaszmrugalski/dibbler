/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelOptInterfaceID.h,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef RELOPTIONINTERFACEID_H
#define RELOPTIONINTERFACEID_H
#include "OptInteger4.h"

class TRelOptInterfaceID : public TOptInteger4
{
  public:
    TRelOptInterfaceID(char * data, int dataLen, TMsg* parent);
    TRelOptInterfaceID(int interfaceID, TMsg* parent);

    bool doDuties();
};

#endif /* RELOPTIONINTERFACEID_H */
