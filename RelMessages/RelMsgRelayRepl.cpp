/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelMsgRelayRepl.cpp,v 1.3 2005-05-02 20:58:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/04/28 21:20:52  thomson
 * Support for multiple relays added.
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#include "Logger.h"
#include "RelMsg.h"
#include "RelMsgRelayRepl.h"
#include "SmartPtr.h"

int TRelMsgRelayRepl::getSize() {
    SmartPtr<TOpt> Option;
    int pktsize=0;
    Options.first();
    while( Option = Options.get() )
	pktsize+=Option->getSize();
    return pktsize + MIN_RELAYREPL_LEN;
}

TRelMsgRelayRepl::TRelMsgRelayRepl(TCtx * ctx, int iface, SmartPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(ctx, iface, addr, 0, 0) // 0,0 - avoid decoding anything
{
    this->MsgType = RELAY_REPL_MSG;
    if (dataLen < MIN_RELAYREPL_LEN) {
	Log(Warning) << "Truncated RELAY_REPL message received." << LogEnd;
	return;
    }

    this->MsgType  = data[0]; // ignored
    this->HopCount = data[1];
    if (this->HopCount >= HOP_COUNT_LIMIT) {
	Log(Warning) << "RelayForw with hopLimit " << this->HopCount << " received (max. allowed is " << HOP_COUNT_LIMIT
		     << ". Message dropped." << LogEnd;
	return;
    }

    this->LinkAddr = new TIPv6Addr(data+2, false);
    this->PeerAddr = new TIPv6Addr(data+18, false);
    data    += 34;
    dataLen -= 34;

    // decode options
    this->decodeOpts(data, dataLen);
}

bool TRelMsgRelayRepl::check() {
    return true;
}

string TRelMsgRelayRepl::getName() {
    return "RELAY_REPL";
}

int TRelMsgRelayRepl::storeSelf(char * buffer)
{
    char *start = buffer;

    *(buffer++) = (char)this->MsgType;
    *(buffer++) = (char)this->HopCount;
    this->LinkAddr->storeSelf(buffer);
    buffer += 16;

    this->PeerAddr->storeSelf(buffer);
    buffer += 16;

    Options.first();
    SmartPtr<TOpt> Option;
    while( Option = Options.get() )
    {
        Option->storeSelf(buffer);
        buffer += Option->getSize();
    }
    return buffer-start;
}
