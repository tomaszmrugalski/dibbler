/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelMsgRelayForw.cpp,v 1.6 2008-08-29 00:07:32 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005-05-02 21:52:39  thomson
 * Logging cleanup.
 *
 * Revision 1.4  2005/05/02 20:58:13  thomson
 * Support for multiple relays added. (bug #107)
 *
 * Revision 1.3  2005/04/28 21:20:52  thomson
 * Support for multiple relays added.
 *
 * Revision 1.2  2005/04/25 00:19:20  thomson
 * Changes in progress.
 *
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

#include "Logger.h"
#include "RelMsg.h"
#include "RelMsgRelayForw.h"

// --- options ---

TRelMsgRelayForw::TRelMsgRelayForw(TCtx * ctx, int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(ctx, iface, addr, 0, 0) // 0,0 - avoid decoding anything
{
    this->MsgType = RELAY_FORW_MSG;
    if (dataLen < MIN_RELAYFORW_LEN) {
	Log(Warning) << "Truncated RELAY_FORW message received." << LogEnd;
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
    Log(Debug) << "Relaying RELAY_FORW with link:" << LinkAddr->getPlain() << ", peer:" << PeerAddr->getPlain() 
               << " and  hop count:" << this->HopCount << "." << LogEnd;
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
    SPtr<TOpt> Option;
    while( Option = Options.get() )
    {
        Option->storeSelf(buffer);
        buffer += Option->getSize();
    }
    return buffer-start;
}

int TRelMsgRelayForw::getSize() {
    SPtr<TOpt> Option;
    int pktsize=0;
    Options.first();
    while( Option = Options.get() )
	pktsize+=Option->getSize();
    return pktsize + MIN_RELAYFORW_LEN;
}
