/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelMsgRelayForw.cpp,v 1.2 2005-04-25 00:19:20 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#include "Logger.h"
#include "RelMsg.h"
#include "RelMsgRelayForw.h"

// --- options ---

TRelMsgRelayForw::TRelMsgRelayForw(TCtx * ctx, int iface, SmartPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(ctx, iface, addr, 0, 0) // 0,0 - avoid decoding anything
{
    this->MsgType = RELAY_FORW_MSG;
    if (dataLen < MIN_RELAYFORW_LEN) {
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
    Log(Debug) << "### link=" << LinkAddr->getPlain() << " peer=" << PeerAddr->getPlain() << LogEnd;
    data    += 34;
    dataLen -= 34;

    // decode options
    this->decodeOpts(data, dataLen);
}

bool TRelMsgRelayForw::check() {
    return true;
}

string TRelMsgRelayForw::getName() {
    return "RELAY_FORW";
}


int TRelMsgRelayForw::storeSelf(char * buffer)
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
