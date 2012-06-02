/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 *
 */

#include "DHCPConst.h"
#include "Opt.h"
#include "OptIAPrefix.h"
#include "SrvOptIAPrefix.h"
#include "OptStatusCode.h"
#include "Msg.h"
#include "Logger.h"
#include "Portable.h"

TSrvOptIAPrefix::TSrvOptIAPrefix( char * buf, int bufsize, TMsg* parent)
    :TOptIAPrefix(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize)
    {
        uint16_t code = readUint16(buf + pos);
        pos += sizeof(uint16_t);
        int length = readUint16(buf + pos);
        pos += sizeof(uint16_t);
        if ((code>0)&&(code<=24))
        {
            if(allowOptInOpt(parent->getType(),OPTION_IAPREFIX,code))
            {
                SPtr<TOpt> opt;
                opt = SPtr<TOpt>();
                switch (code)
                    {
                    case OPTION_STATUS_CODE:
                        opt =(Ptr*)SPtr<TOptStatusCode> (new TOptStatusCode(buf+pos,length,this->Parent));
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
            Log(Warning) <<"Unknown option in option IAPREFIX(optType="
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
