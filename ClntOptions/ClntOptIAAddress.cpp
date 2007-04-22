/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptIAAddress.cpp,v 1.6 2007-04-22 21:19:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.5.2.1  2007-04-15 21:23:31  thomson
 * AddrParams implemented (prefix part)
 *
 * Revision 1.5  2006-10-06 00:42:13  thomson
 * Initial PD support.
 *
 * Revision 1.4  2004-12-02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.3  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
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

TClntOptIAAddress::TClntOptIAAddress( char * buf, int n, TMsg* parent)
	:TOptIAAddress(buf, n, parent)
{
 
	int pos=0;
    while(pos<n) 
    {
        int code   = ntohs( *(short*)buf);
        pos+=2;
        int length = ntohs( *(short*)buf);
        pos+=2;
	if(allowOptInOpt(parent->getType(),OPTION_IAADDR,code))
	{
	    SmartPtr<TOpt> opt;
	    switch (code)
	    {
	    case OPTION_STATUS_CODE:
		SubOptions.append( new TClntOptStatusCode(buf+pos,length, this->Parent) );
		break;
	    case OPTION_ADDRPARAMS:
		SubOptions.append( new TClntOptAddrParams(buf+pos, length, this->Parent));
		Log(Debug) << "AddrParams option received." << LogEnd;
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
	else
	    Log(Warning) << "Illegal option received, opttype=" << code 
			 << " in field options of IA_NA option"<<LogEnd;
	pos+=length;
    }
}

TClntOptIAAddress::TClntOptIAAddress( SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent)
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
