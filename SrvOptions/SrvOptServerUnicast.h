/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptServerUnicast.h,v 1.2 2004-09-05 15:27:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#ifndef SRVOPTSERVERUNICAST_H
#define SRVOPTSERVERUNICAST_H

#include "Opt.h"
#include "DHCPConst.h"
#include "OptServerUnicast.h"

class TSrvOptServerUnicast : public TOptServerUnicast {
 public:
    TSrvOptServerUnicast( char * buf,  int n, TMsg* parent);
    TSrvOptServerUnicast(SmartPtr<TIPv6Addr> addr, TMsg* parent);
    bool doDuties();	
};



#endif 
