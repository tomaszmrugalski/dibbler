/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptInterfaceID.cpp,v 1.5 2008-08-29 00:07:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006-10-29 13:11:47  thomson
 * Size of the Elapsed time option has been corrected,
 * OptInteger4 class migrated to OptInteger.
 *
 * Revision 1.3  2005-01-08 16:52:04  thomson
 * Relay support implemented.
 *
 */

#include "SrvOptInterfaceID.h"
#include "DHCPConst.h"

TSrvOptInterfaceID::TSrvOptInterfaceID( char * buf,  int n, TMsg* parent)
    :TOptInteger(OPTION_INTERFACE_ID, 4 /* FIXME */, buf,n, parent) {
}

bool TSrvOptInterfaceID::doDuties() {
    return true;
}
