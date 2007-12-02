/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ReqOpt.cpp,v 1.1 2007-12-02 10:32:00 thomson Exp $
 *
 */
#include "ReqOpt.h"

TReqOptAddr::TReqOptAddr(int type, SmartPtr<TIPv6Addr> addr, TMsg * parent)
    :TOptAddr(type, addr, parent)
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
