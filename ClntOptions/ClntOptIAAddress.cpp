/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptIAAddress.cpp,v 1.8 2008-08-29 00:07:29 thomson Exp $
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
#include "OptIAAddress.h"
#include "ClntOptIAAddress.h"
#include "ClntOptStatusCode.h"
#include "ClntOptAddrParams.h"
#include "Logger.h"
#include "IPv6Addr.h"
#include "Msg.h"

TClntOptIAAddress::TClntOptIAAddress( char * buf, int bufSize, TMsg* parent)
	:TOptIAAddress(buf, bufSize, parent)
{
    SPtr<TOpt> opt = 0;
    int MsgType = 0;
    if (parent)
	parent->getType();
    
    int pos=0;
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

	if(allowOptInOpt(parent->getType(),OPTION_IAADDR,code))
	{
	    opt = 0;
	    switch (code)
	    {
	    case OPTION_STATUS_CODE:
		opt = new TClntOptStatusCode(buf+pos,length, this->Parent);
		break;
	    case OPTION_ADDRPARAMS:
		opt = new TClntOptAddrParams(buf+pos, length, this->Parent);
		Log(Debug) << "AddrParams option received." << LogEnd;
		break;
	    default:
		Log(Warning) <<"Suboption (type=" << code<< ") not supported "
			     <<" in IAADDR option in message (type="<< parent->getType() 
			     <<")." << LogEnd;
		break;
	    }
	    if((opt)&&(opt->isValid()))
		SubOptions.append(opt);
	}
	else
	    Log(Warning) << "Illegal option received, opttype=" << code 
			 << " in field options of IA_NA option"<<LogEnd;
	pos+=length;
    }
}

TClntOptIAAddress::TClntOptIAAddress( SPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent)
	:TOptIAAddress(addr,pref,valid, parent)
{

}

bool TClntOptIAAddress::doDuties()
{
    return false;
}

bool TClntOptIAAddress::isValid()
{
    if (TOptIAAddress::isValid())
        return this->getValid()>=this->getPref();
    else
        return false;

}
