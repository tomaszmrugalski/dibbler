/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvOptIAAddress.h,v 1.2 2004-06-17 23:53:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#ifndef SRVOPTIAADDRESS_H
#define SRVOPTIAADDRESS_H

#include "SmartPtr.h"
#include "Container.h"
#include "OptIAAddress.h"

class TSrvOptIAAddress : public TOptIAAddress {
 public:
    TSrvOptIAAddress( char * addr, int n, TMsg* parent);
    
    TSrvOptIAAddress( SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent);

    bool doDuties();
};

#endif
