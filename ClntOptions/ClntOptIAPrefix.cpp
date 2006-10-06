/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * 
 * released under GNU GPL v2 or later licence
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

TClntOptIAPrefix::TClntOptIAPrefix( char * buf, int n, TMsg* parent)
	:TOptIAPrefix(buf, n, parent)
{

   int pos=0;
   while(pos<n) 
    {
        int code   = ntohs( *(short*)buf);
        pos+=2;
	int length = ntohs( *(short*)buf);
        pos+=2;
        if ((code>0)&&(code<=26))// was 24 but new options were declared IAPrefix is 26
        {                
            if(allowOptInOpt(parent->getType(),OPTION_IAPREFIX,code))
            {
                printf("Got option IA_PREFIX\n");
		 SmartPtr<TOpt> opt;
                switch (code)
                {
                case OPTION_STATUS_CODE:
                    SubOptions.append( new TClntOptStatusCode(buf+pos,length, this->Parent) );
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
			     << " in field options of IA_Prefix option"<<LogEnd;
        }
        else
        {
	    Log(Warning) <<"Unknown option in option IA_Prefix( optType=" 
			 << code << "). Option ignored." << LogEnd;
        };
	    pos+=length;
    }
}

TClntOptIAPrefix::TClntOptIAPrefix( SmartPtr<TIPv6Addr> addr, long pref, long valid, char prefixLength, TMsg* parent)
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
