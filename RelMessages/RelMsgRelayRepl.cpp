/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "Logger.h"
#include "RelMsg.h"
#include "RelMsgRelayRepl.h"
#include "SmartPtr.h"

int TRelMsgRelayRepl::getSize() {
    int pktsize=0;
    TOptList::iterator opt;
    for (opt = Options.begin(); opt!=Options.end(); ++opt)
    {
	pktsize += (*opt)->getSize();
    }
    return pktsize + MIN_RELAYREPL_LEN;
}

TRelMsgRelayRepl::TRelMsgRelayRepl(int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(iface, addr, 0, 0) // 0,0 - avoid decoding anything
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


    TOptList::iterator option;
    for (option=Options.begin(); option!=Options.end(); ++option) {
        (*option)->storeSelf(buffer);
	buffer += (*option)->getSize();
    }
    return buffer-start;
}
