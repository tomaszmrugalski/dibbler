/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "OptAddrParams.h"
#include "DHCPConst.h"

TOptAddrParams::TOptAddrParams(const char *buf, size_t len, TMsg *parent)
    : TOptInteger(OPTION_ADDRPARAMS, 2, buf, len, parent) {}

int TOptAddrParams::getPrefix() { return (Value >> 8) & 0xff; }

int TOptAddrParams::getBitfield() { return Value & 0xff; }

bool TOptAddrParams::doDuties() { return true; }
