/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptInterfaceID.h,v 1.5 2008-11-11 22:41:50 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    TSrvOptInterfaceID( char * buf,  int n, TMsg* parent);
    bool doDuties();
};



#endif /* OPTIONINTERFACEID_H */
