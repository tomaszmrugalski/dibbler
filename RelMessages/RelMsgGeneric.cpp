/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include <iostream>
#include "RelMsgGeneric.h"

using namespace std;

TRelMsgGeneric::TRelMsgGeneric(int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(iface, addr, data, dataLen) {

}

bool TRelMsgGeneric::check() {
    return true;
}

string TRelMsgGeneric::getName() {
    switch (this->MsgType) {
    case SOLICIT_MSG:             return "SOLICIT";
    case ADVERTISE_MSG:           return "ADVERTISE";
    case REQUEST_MSG:             return "REQUEST";
    case CONFIRM_MSG:             return "CONFIRM";
    case RENEW_MSG:               return "RENEW";
    case REBIND_MSG:              return "REBIND";
    case REPLY_MSG:               return "REPLY";
    case RELEASE_MSG:             return "RELEASE";
    case DECLINE_MSG:             return "DECLINE";
    case RECONFIGURE_MSG:         return "RECONFIGURE";
    case INFORMATION_REQUEST_MSG: return "INF-REQUEST";
    case RELAY_FORW_MSG:          return "RELAY_FORW";
    case RELAY_REPL_MSG:          return "RELAY_REPL";
    default:
	return "UNKNOWN/GENERIC";
    }
}
