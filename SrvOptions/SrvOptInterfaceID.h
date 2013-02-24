/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef SRVOPTIONINTERFACEID_H
#define SRVOPTIONINTERFACEID_H
#include "OptGeneric.h"

class TSrvOptInterfaceID : public TOptGeneric
{

  public:
    bool operator==(const TSrvOptInterfaceID &other) const;
    TSrvOptInterfaceID(int id, TMsg * parent);
    TSrvOptInterfaceID(const char * buf,  int n, TMsg* parent);
    bool doDuties();
};



#endif /* OPTIONINTERFACEID_H */
