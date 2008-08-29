/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ReqMsg.h,v 1.3 2008-08-29 00:07:33 thomson Exp $
 *
 */

#ifndef REQMSG_H
#define REQMSG_H

#include <string>
#include "Msg.h"
#include "SmartPtr.h"

class TReqMsg: public TMsg
{
public:
    TReqMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType);
    // used to create TMsg object based on received char[] data
    TReqMsg(int iface, SmartPtr<TIPv6Addr> addr, char* &buf, int &bufSize);
    void addOption(SPtr<TOpt> opt);

    string getName();
};

#endif
