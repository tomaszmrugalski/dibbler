/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Grzegorz Pluto
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <stdlib.h>
#include <iostream>
#include <string>
#include "Logger.h"

#ifdef WIN32
#include <winsock2.h>
#endif
#if defined(LINUX) || defined(BSD)
#include <arpa/inet.h>
#endif

#include "DHCPConst.h"
#include "OptReconfigureMsg.h"

TOptReconfigureMsg::TOptReconfigureMsg(int msgType, TMsg* parent)
    :TOpt(OPTION_RECONF_MSG, parent), MsgType_(msgType)
{
}

TOptReconfigureMsg::TOptReconfigureMsg(char *buf, int bufsize, TMsg* parent)
    :TOpt(OPTION_RECONF_MSG, parent)
{
    if ((unsigned int)bufsize != 1) {
        Valid = false;
        return;
    }
    MsgType_ = (unsigned char)(buf[0]);
    if( (MsgType_ != 5) &&  // renew, as defined in RFC3315
        (MsgType_ != 11) && // inf-request, as defined in RFC3315
        (MsgType_ != 6) )  { //rebind, as defined in draft-ietf-dhc-dhcpv6-reconfigure-rebind-08
        Log(Warning) << "Invalid content of reconfigure option. Message type "
                     << (int)MsgType_ << " not valid (only 5,7 and 11 are)." << LogEnd;
        Valid = false;
        return;
    }
}

size_t TOptReconfigureMsg::getSize()
{
    return 4 + 1; // header (4) + data-length (1)
}

char * TOptReconfigureMsg::storeSelf( char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(1); // length
    buf+=2;
    *buf = (char)MsgType_;
    return buf+1;
}

bool TOptReconfigureMsg::isValid() const
{
    if ( MsgType_==RENEW_MSG || MsgType_==INFORMATION_REQUEST_MSG || MsgType_==REBIND_MSG )
        return true;
    return false;
}
