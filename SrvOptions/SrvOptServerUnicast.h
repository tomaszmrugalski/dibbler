/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptServerUnicast.h,v 1.3 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 *
 */
#ifndef SRVOPTSERVERUNICAST_H
#define SRVOPTSERVERUNICAST_H

#include "Opt.h"
#include "DHCPConst.h"
#include "OptAddr.h"

class TSrvOptServerUnicast : public TOptAddr {
 public:
    TSrvOptServerUnicast( char * buf,  int n, TMsg* parent);
    TSrvOptServerUnicast(SmartPtr<TIPv6Addr> addr, TMsg* parent);
    bool doDuties();	
};



#endif 
