/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptIAAddress.h,v 1.5 2008-08-29 00:07:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004-10-27 22:07:56  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.3  2004/09/07 22:02:34  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.2  2004/06/17 23:53:55  thomson
 * Server Address Assignment rewritten.
 */

#ifndef SRVOPTIAADDRESS_H
#define SRVOPTIAADDRESS_H

#include "SmartPtr.h"
#include "Container.h"
#include "OptIAAddress.h"

class TSrvOptIAAddress : public TOptIAAddress {
 public:
    TSrvOptIAAddress( char * addr, int n, TMsg* parent);
    
    TSrvOptIAAddress(SPtr<TIPv6Addr> addr, unsigned long pref, 
		     unsigned long valid, TMsg* parent);

    bool doDuties();
};

#endif
