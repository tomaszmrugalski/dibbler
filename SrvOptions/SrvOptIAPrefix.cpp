/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                 
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
#include "OptIAPrefix.h"
#include "SrvOptIAPrefix.h"
#include "SrvOptStatusCode.h"
#include "Msg.h"
#include "Logger.h"

TSrvOptIAPrefix::TSrvOptIAPrefix( char * buf, int bufsize, TMsg* parent)
    :TOptIAPrefix(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize) 
    {
        int code=buf[pos]*256+buf[pos+1]; /// @todo: use htons!
        pos+=2;
        int length=buf[pos]*256+buf[pos+1]; /// @todo: use htons!
        pos+=2;
        if ((code>0)&&(code<=24))
        {                
            if(allowOptInOpt(parent->getType(),OPTION_IAPREFIX,code))
            {
                SPtr<TOpt> opt;
		            opt = SPtr<TOpt>();
                switch (code)
                {
                case OPTION_STATUS_CODE:
                    opt =(Ptr*)SPtr<TSrvOptStatusCode> (new TSrvOptStatusCode(buf+pos,length,this->Parent));
                    break;
                default:
		                  Log(Warning) << "Option " << code<< " not supported "
                                   << " in message (type="<< parent->getType() <<")." << LogEnd;
                    break;
                }
                if((opt)&&(opt->isValid()))
                    SubOptions.append(opt);
            } else {
		            Log(Warning) << "Illegal option received, opttype=" << code 
			                 << " in field options of IA_PD option" << LogEnd;
      	    }
        } else {
	          Log(Warning) <<"Unknown option in option IA_PD( optType=" 
		       << code << "). Option ignored." << LogEnd;
        };
        pos+=length;
    }
}

TSrvOptIAPrefix::TSrvOptIAPrefix(SPtr<TIPv6Addr> prefix, char length, unsigned long pref, 
				   unsigned long valid, TMsg* parent)
    :TOptIAPrefix(prefix,length,pref,valid, parent) {
	
}

bool TSrvOptIAPrefix::doDuties() {
    return true;
}
