/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
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
    TReqMsg(int iface, SPtr<TIPv6Addr> addr, int msgType);

    // used to create TMsg object based on received char[] data
    TReqMsg(int iface, SPtr<TIPv6Addr> addr, char* &buf, int &bufSize);

    //constructor for bulk leasequery messages
    TReqMsg(int iface, SPtr<TIPv6Addr> addr,int msgSize, int msgType);

    void addOption(SPtr<TOpt> opt);

    std::string getName() const;

    void validateLQ();//defined in rfc 5007 (why validate ?)
    void isComletion(); //check completion of server replay (rfc 5460 p.11)
    void multipleQuery();//requestor can send multiple query to receive more data
};

#endif
