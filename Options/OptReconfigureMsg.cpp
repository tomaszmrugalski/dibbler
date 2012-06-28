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
    :TOpt(OPTION_RECONF_MSG, parent)
{
    this->MSG_Type=msgType;
}

TOptReconfigureMsg::TOptReconfigureMsg(char *buf, int bufsize, TMsg* parent)
    :TOpt(OPTION_RECONF_MSG, parent)
{
    if ((unsigned int)bufsize<1) {
        Valid = false;
        return;
    }
    this->MSG_Type = (unsigned char)(buf[0]);
    if(MSG_Type!=5 /*renew, as defined in RFC3315 */
       && MSG_Type!=11 /*inf-request, as defined in RFC3315 */
       && MSG_Type!=6 /* rebind, as defined in draft-ietf-dhc-dhcpv6-reconfigure-rebind-08 */) {
        Log(Warning) << "Invalid content of reconfigure option. Message type "
                     << (int)MSG_Type << " not valid (only 5,7 and 11 are)." << LogEnd;
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
    *buf = (char)this->MSG_Type;
    return buf+1;
}

bool TOptReconfigureMsg::isValid()
{
    if ( MSG_Type==RENEW_MSG || MSG_Type==INFORMATION_REQUEST_MSG || MSG_Type==REBIND_MSG )
        return true;
    return false;
}
