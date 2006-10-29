/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptInterfaceID.h,v 1.3 2006-10-29 13:11:47 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005-01-08 16:52:04  thomson
 * Relay support implemented.
 *
 */

#ifndef SRVOPTIONINTERFACEID_H
#define SRVOPTIONINTERFACEID_H
#include "OptInteger.h"

class TSrvOptInterfaceID : public TOptInteger
{
  public:
    TSrvOptInterfaceID( char * buf,  int n, TMsg* parent);
	bool doDuties();
};



#endif /* OPTIONINTERFACEID_H */
