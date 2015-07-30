/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "Portable.h"
#include "Logger.h"
#include "RelMsg.h"
#include "OptDUID.h"
#include "RelOptInterfaceID.h"
#include "RelOptRelayMsg.h"
#include "RelOptGeneric.h"

//Constructor builds message on the basis of received message
TRelMsg::TRelMsg(int iface,  SPtr<TIPv6Addr> addr, char* data,  int dataLen)
    :TMsg(iface, addr, data, dataLen), DestIface(0) {
    // data+=4, dataLen-=4 is modified in TMsg
    if (dataLen<=0) // avoid decoding of empty messages.
        return;

    this->HopCount = 0;
    this->decodeOpts(data, dataLen);
}

void TRelMsg::decodeOpts(char * buf, int bufSize) {
    int pos=0;
    SPtr<TOpt> ptr;

    while (pos<bufSize)	{
	if (pos+4>bufSize) {
	    Log(Error) << "Message " << MsgType << " truncated. There are " << (bufSize-pos) 
		       << " bytes left to parse. Bytes ignored." << LogEnd;
	    break;
	}
        unsigned short code   = readUint16(buf+pos);
        pos += sizeof(uint16_t);
        unsigned short length = readUint16(buf+pos);
        pos += sizeof(uint16_t);
	if (pos+length>bufSize) {
	    Log(Error) << "Invalid option (type=" << code << ", len=" << length 
		       << " received (msgtype=" << MsgType << "). Message dropped." << LogEnd;
	    IsDone = true;
	    return;
	}

	if (!allowOptInMsg(this->MsgType,code)) {
	    Log(Warning) << "Option " << code << " not allowed in message type="<< MsgType <<". Ignoring." << LogEnd;
	    pos+=length;
	    continue;
	}
	if (!allowOptInOpt(this->MsgType,0,code)) {
	    Log(Warning) <<"Option " << code << " can't be present in message (type="
			 << MsgType <<") directly. Ignoring." << LogEnd;
	    pos+=length;
	    continue;
	}

	ptr.reset();
	switch (code) {
	case OPTION_RELAY_MSG:
	    ptr = new TRelOptRelayMsg(buf+pos,length,this);
	    break;
	case OPTION_INTERFACE_ID:
	    ptr = new TRelOptInterfaceID(buf+pos,length,this);
	    break;
        case OPTION_CLIENTID:
            ptr = new TOptDUID(code, buf+pos, length, this);
            break;
	default:
	    ptr = new TRelOptGeneric(code, buf+pos, length, this);
	    break;
	}

	if ( (ptr) && (ptr->isValid()) )
	    Options.push_back( ptr );
	else
	    Log(Warning) << "Option " << code << " is invalid. Option ignored." << LogEnd;
        pos+=length;
    }
}

void TRelMsg::setDestination(int iface, SPtr<TIPv6Addr> dest) {
    this->DestIface = iface;
    this->DestAddr  = dest;
}

int TRelMsg::getDestIface() {
    return this->DestIface;
}

SPtr<TIPv6Addr> TRelMsg::getDestAddr() {
    return this->DestAddr;
}

int TRelMsg::getHopCount() {
    return this->HopCount;
}
