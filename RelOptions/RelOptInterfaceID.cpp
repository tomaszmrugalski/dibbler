/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelOptInterfaceID.cpp,v 1.1 2005-01-11 22:53:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "RelOptInterfaceID.h"
#include "DHCPConst.h"

TRelOptInterfaceID::TRelOptInterfaceID(char * data, int dataLen, TMsg* parent)
	:TOptInteger4(OPTION_INTERFACE_ID, data, dataLen, parent) {

}

TRelOptInterfaceID::TRelOptInterfaceID(int interfaceID, TMsg* parent)
    :TOptInteger4(OPTION_INTERFACE_ID, interfaceID, parent) {

}

bool TRelOptInterfaceID::doDuties() {
    return true;
}
