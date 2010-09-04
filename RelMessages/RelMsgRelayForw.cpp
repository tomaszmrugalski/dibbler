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
#include "RelMsgRelayForw.h"

// --- options ---

TRelMsgRelayForw::TRelMsgRelayForw(int iface, SPtr<TIPv6Addr> addr, char * data, int dataLen)
    :TRelMsg(iface, addr, 0, 0) // 0,0 - avoid decoding anything
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

    TOptList::iterator option;
    for (option=Options.begin(); option!=Options.end(); ++option) {
        (*option)->storeSelf(buffer);
	buffer += (*option)->getSize();
    }
    return buffer-start;
}

int TRelMsgRelayForw::getSize() {
    int pktsize=0;
    TOptList::iterator opt;
    for (opt = Options.begin(); opt!=Options.end(); ++opt)
    {
	pktsize += (*opt)->getSize();
    }
    return pktsize + MIN_RELAYFORW_LEN;
}
