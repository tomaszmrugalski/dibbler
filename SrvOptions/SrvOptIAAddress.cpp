/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvOptIAAddress.cpp,v 1.2 2004-06-17 23:53:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
#include "SrvOptIAAddress.h"
#include "SrvOptStatusCode.h"
#include "Msg.h"
#include "Logger.h"

TSrvOptIAAddress::TSrvOptIAAddress( char * buf, int bufsize, TMsg* parent)
    :TOptIAAddress(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize) 
    {
        int code=buf[pos]*256+buf[pos+1]; // FIXME: use htons!
        pos+=2;
        int length=buf[pos]*256+buf[pos+1]; // FIXME: use htons!
        pos+=2;
        if ((code>0)&&(code<=24))
        {                
            if(canBeOptInOpt(parent->getType(),OPTION_IAADDR,code))
            {
                SmartPtr<TOpt> opt;
		opt = SmartPtr<TOpt>();
                switch (code)
                {
                case OPTION_STATUS_CODE:
                    opt =(Ptr*)SmartPtr<TSrvOptStatusCode> (
			new TSrvOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
		    Log(Warning) << "Option " << code<< " not supported "
                        <<" in message (type="<< parent->getType() <<")." << LogEnd;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
            } else {
		Log(Warning) << "Illegal option received, opttype=" << code 
			     << " in field options of IA_NA option" << LogEnd;
	    }
        } else {
	    Log(Warning) <<"Unknown option in option IA_NA( optType=" 
		 << code << "). Option ignored." << LogEnd;
        };
        pos+=length;
    }
}

TSrvOptIAAddress::TSrvOptIAAddress( SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent)
	:TOptIAAddress(addr,pref,valid, parent) {

}
bool TSrvOptIAAddress::doDuties() {
    return true;
}
