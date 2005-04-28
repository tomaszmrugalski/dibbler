/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelMsgGeneric.cpp,v 1.3 2005-04-28 21:20:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/01/13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.1  2005/01/12 00:00:38  thomson
 * *** empty log message ***
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#include <iostream>
#include "RelMsgGeneric.h"

using namespace std;

TRelMsgGeneric::TRelMsgGeneric(TCtx * ctx, int iface, SmartPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(ctx, iface, addr, data, dataLen) {

}

bool TRelMsgGeneric::check() {
    // FIXME
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

int TRelMsgGeneric::getSize() {
    SmartPtr<TOpt> Option;
    int pktsize=0;
    Options.first();
    while( Option = Options.get() )
	pktsize+=Option->getSize();
    return pktsize + 4;
}
