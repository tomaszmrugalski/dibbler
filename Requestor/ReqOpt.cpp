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



bool TReqOptRemoteId::doDuties()
{
    return true;
}

char *TReqOptRemoteId::storeSelf(char *buf, int queryType, int enterpriseNum)
{
    // option-code OPTION_VENDOR_OPTS (2 bytes long)
    buf = writeUint16(buf,queryType);

    // option-len size of total option-data
    buf = writeUint16(buf, getSize()-4);

    // enterprise-number (4 bytes long)


    //TODO: conversion of enterprise number should be here:

    int tmp=0;
    buf = writeUint32(buf,tmp);

    SPtr<TOpt> opt;
    firstOption();

    while (opt = getOption())
    {
        buf = opt->storeSelf(buf);
    }
    buf = storeSubOpt(buf);
    return buf;
}

 //TOptVendorSpecInfo(int type, int enterprise, char *data, int dataLen, TMsg* parent);
TReqOptRemoteId::TReqOptRemoteId(int type, char *remoteId, int enterprise,char * data,  int dataLen, TMsg *parent):TOptVendorSpecInfo(type,  enterprise, data, dataLen, parent) {
   this->remoreIdRqOpt=remoteId;
}

size_t TReqOptRemoteId::getSize()
{
    SPtr<TOpt> opt;
    unsigned int len = 8; // normal header(4) + remoteId(4)
    firstOption();
    while (opt = getOption()) {
        len += opt->getSize();
    }
    if (len < 5 ) {
        Log(Debug) << "Option-len filed in remoteId option is to short." <<LogEnd;
    }
    return len;
}

TReqOptRelayId::TReqOptRelayId(int type, SPtr<TDUID> duid, TMsg *parent)
    :TOptDUID(type, duid, parent)
{


}

