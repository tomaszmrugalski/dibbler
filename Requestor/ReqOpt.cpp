/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ReqOpt.cpp,v 1.3 2008-08-29 00:07:33 thomson Exp $
 *
 */
#include "ReqOpt.h"
#include "SmartPtr.h"
#include "OptDUID.h"
#include "OptVendorSpecInfo.h"

TReqOptAddr::TReqOptAddr(int type, SPtr<TIPv6Addr> addr, TMsg * parent)
  :TOptIAAddress(addr, 0x33333333, 0x88888888, parent)
{

    //use 0x33333333 as preffered lifetime and 0x88888888 as valid lifetime value ?
}

bool TReqOptAddr::doDuties()
{
    return true;
}



TReqOptDUID::TReqOptDUID(int type, SPtr<TDUID> duid, TMsg* parent)
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

//TReqOptGeneric::TReqOptGeneric(int optType, char * data, int dataLen, TMsg* parent, int remoteId)
//    :TOptGeneric(optType, data, dataLen, parent)
//{

//}

bool TReqOptGeneric::doDuties()
{
    return true;
}



bool TReqOptRelayId::doDuties()
{
    return true;
}

TReqOptRelayId::TReqOptRelayId(int type, SPtr<TDUID> duid, TMsg *parent)
    :TOptDUID(type, duid, parent)
{


}

