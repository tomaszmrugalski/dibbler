/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ReqOpt.cpp,v 1.2 2007-12-03 16:59:17 thomson Exp $
 *
 */
#include "ReqOpt.h"

TReqOptAddr::TReqOptAddr(int type, SmartPtr<TIPv6Addr> addr, TMsg * parent)
  :TOptIAAddress(addr, 0x33333333, 0x88888888, parent)
{
}

bool TReqOptAddr::doDuties()
{
    return true;
}

TReqOptDUID::TReqOptDUID(int type, SmartPtr<TDUID> duid, TMsg* parent)
    :TOptDUID(type, duid, parent)
{
}

bool TReqOptDUID::doDuties()
{
    return true;
}

TReqOptGeneric::TReqOptGeneric(int optType, char * data, int dataLen, TMsg* parent)
    :TOptGeneric(optType, data, dataLen, parent)
{
}

bool TReqOptGeneric::doDuties()
{
    return true;
}
