/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelOptInterfaceID.cpp,v 1.2 2006-10-29 13:11:46 thomson Exp $
 *
 */

#include "RelOptInterfaceID.h"
#include "DHCPConst.h"

TRelOptInterfaceID::TRelOptInterfaceID(char * data, int dataLen, TMsg* parent)
    :TOptInteger(OPTION_INTERFACE_ID, 4 /* FIXME: Support length other than 4 */, data, dataLen, parent) {
}

TRelOptInterfaceID::TRelOptInterfaceID(int interfaceID, TMsg* parent)
    :TOptInteger(OPTION_INTERFACE_ID, 4 /* FIXME: Support length other than 4 */, interfaceID, parent) {
}

bool TRelOptInterfaceID::doDuties() {
    return true;
}
