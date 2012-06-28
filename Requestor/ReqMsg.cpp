/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ReqMsg.cpp,v 1.2 2007-12-03 16:59:17 thomson Exp $
 */

#include <string>
#include "ReqMsg.h"
#include "DHCPConst.h"

using namespace std;

TReqMsg::TReqMsg(int iface, SPtr<TIPv6Addr> addr, int msgType)
    :TMsg(iface, addr, msgType)
{

}
    // used to create TMsg object based on received char[] data
TReqMsg::TReqMsg(int iface, SPtr<TIPv6Addr> addr, char* &buf, int &bufSize)
    :TMsg(iface, addr, buf, bufSize)
{

}


std::string TReqMsg::getName() const
{
    switch (MsgType) {
    case LEASEQUERY_MSG:
        return "LEASEQUERY";    
    case LEASEQUERY_REPLY_MSG:
        return "LEASEQUERY_RSP";
    default:
        return "unknown";
    }
}

void TReqMsg::addOption(SPtr<TOpt> opt)
{
    Options.push_back(opt);
}

