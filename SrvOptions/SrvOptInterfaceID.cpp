/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptInterfaceID.cpp,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptInterfaceID.h"
#include "DHCPConst.h"

TSrvOptInterfaceID::TSrvOptInterfaceID( char * buf,  int n, TMsg* parent)
	:TOptInteger4(OPTION_INTERFACE_ID, buf,n, parent) {

}

bool TSrvOptInterfaceID::doDuties() {
    return true;
}
