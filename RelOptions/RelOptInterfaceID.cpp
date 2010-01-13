/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelOptInterfaceID.cpp,v 1.3 2008-08-29 00:07:32 thomson Exp $
 *
 */

#include "RelOptInterfaceID.h"
#include "DHCPConst.h"

TRelOptInterfaceID::TRelOptInterfaceID(char * data, int dataLen, TMsg* parent)
    :TOptInteger(OPTION_INTERFACE_ID, 4 /** @todo: Support length other than 4 */, data, dataLen, parent) {
}

TRelOptInterfaceID::TRelOptInterfaceID(int interfaceID, TMsg* parent)
    :TOptInteger(OPTION_INTERFACE_ID, 4 /** @todo: Support length other than 4 */, interfaceID, parent) {
}

bool TRelOptInterfaceID::doDuties() {
    return true;
}
