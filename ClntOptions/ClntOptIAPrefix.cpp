/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * 
 * released under GNU GPL v2 only licence
 *
 */

#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 

#include "DHCPConst.h"
#include "Opt.h"
#include "OptIAPrefix.h"
#include "ClntOptIAPrefix.h"
#include "ClntOptStatusCode.h"
#include "Logger.h"
#include "IPv6Addr.h"
#include "Msg.h"

TClntOptIAPrefix::TClntOptIAPrefix( char * buf, int bufSize, TMsg* parent)
	:TOptIAPrefix(buf, bufSize, parent)
{
    SPtr<TOpt> opt = 0;
    int pos=0;
    int MsgType = 0;
    if (parent)
	MsgType = parent->getType();

    while(pos<bufSize) 
    {
	if (pos+4>bufSize) {
	    Log(Error) << "Message " << MsgType << " truncated. There are " << (bufSize-pos) 
		       << " bytes left to parse. Bytes ignored." << LogEnd;
	    break;
	}
        unsigned short code   = ntohs( *((unsigned short*) (buf+pos)));
        pos+=2;
        unsigned short length = ntohs( *((unsigned short*) (buf+pos)));
        pos+=2;
	if (pos+length>bufSize) {
	    Log(Error) << "Invalid option (type=" << code << ", len=" << length 
		       << " received (msgtype=" << MsgType << "). Message dropped." << LogEnd;
	    return;
	}
	
	if (allowOptInOpt(parent->getType(),OPTION_IAPREFIX,code))
	{
	    switch (code)
	    {
	    case OPTION_STATUS_CODE:
		opt = new TClntOptStatusCode(buf+pos,length, this->Parent);
		break;
	    default:
		Log(Warning) <<"Option opttype=" << code<< "not supported "
			     <<" in field of message (type="<< parent->getType() 
			     <<") in this version of client."<<LogEnd;
		break;
	    }
	    if((opt)&&(opt->isValid()))
		SubOptions.append(opt);
	}
	pos += length;
    }
}

TClntOptIAPrefix::TClntOptIAPrefix( SPtr<TIPv6Addr> addr, long pref, long valid, char prefixLength, TMsg* parent)
	:TOptIAPrefix(addr,prefixLength,pref,valid, parent)
{

}

bool TClntOptIAPrefix::doDuties()
{
    return false;
}

bool TClntOptIAPrefix::isValid()
{
    if (TOptIAPrefix::isValid())
        return this->getValid()>=this->getPref();
    else
        return false;

}
