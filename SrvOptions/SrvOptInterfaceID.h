/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptInterfaceID.h,v 1.2 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTIONINTERFACEID_H
#define SRVOPTIONINTERFACEID_H
#include "OptInteger4.h"

class TSrvOptInterfaceID : public TOptInteger4
{
  public:
    TSrvOptInterfaceID( char * buf,  int n, TMsg* parent);
	bool doDuties();
};



#endif /* OPTIONINTERFACEID_H */
