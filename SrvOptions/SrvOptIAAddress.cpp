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

        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
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
                    opt =(Ptr*)SmartPtr<TSrvOptStatusCode> (new TSrvOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
                    clog<< logger::logWarning <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< parent->getType() <<") in this version of server."<<logger::endl;
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

TSrvOptIAAddress::TSrvOptIAAddress( SmartPtr<TIPv6Addr> addr, long pref, long valid, TMsg* parent)
	:TOptIAAddress(addr,pref,valid, parent)
{

}
bool TSrvOptIAAddress::doDuties()
{
    return true;
}
