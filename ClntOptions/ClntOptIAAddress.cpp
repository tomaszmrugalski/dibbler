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
        if ((code>0)&&(code<=24))
        {                
            if(canBeOptInOpt(parent->getType(),OPTION_IAADDR,code))
            {
                SmartPtr<TOpt> opt;
                switch (code)
                {
                case OPTION_STATUS_CODE:
                    SubOptions.append( new TClntOptStatusCode(buf+pos,length, this->Parent) );
                    break;
                default:
                    clog<< logger::logWarning <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< parent->getType() <<") in this version of client."<<logger::endl;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
            }
            else
                clog << logger::logWarning << "Illegal option received, opttype=" << code 
                    << " in field options of IA_NA option"<<logger::endl;
        }
        else
        {
                clog << logger::logWarning <<"Unknown option in option IA_NA( optType=" 
                    << code << "). Option ignored." << logger::endl;
        };
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
