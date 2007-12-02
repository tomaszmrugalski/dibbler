/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ReqMsg.h,v 1.1 2007-12-02 10:31:59 thomson Exp $
 *
 */

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