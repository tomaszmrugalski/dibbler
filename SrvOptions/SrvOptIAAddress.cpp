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
#include "DHCPConst.h"
#include "Opt.h"
#include "OptIAAddress.h"
#include "SrvOptIAAddress.h"
#include "OptStatusCode.h"
#include "Msg.h"
#include "Logger.h"

TSrvOptIAAddress::TSrvOptIAAddress( char * buf, int bufsize, TMsg* parent)
    :TOptIAAddress(buf,bufsize, parent)
{
    int pos=0;
    while(pos<bufsize)
    {
        uint16_t code = readUint16(buf+pos);
        pos += sizeof(uint16_t);
        uint16_t length = readUint16(buf+pos);
        pos += sizeof(uint16_t);
        if ((code>0)&&(code<=24))
        {
            if(allowOptInOpt(parent->getType(),OPTION_IAADDR,code))
            {
                SPtr<TOpt> opt;
                opt = SPtr<TOpt>();
                switch (code)
                {
                case OPTION_STATUS_CODE:
                    opt =(Ptr*)SPtr<TOptStatusCode> (
                        new TOptStatusCode(buf+pos, length, Parent));
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
            Log(Warning) <<"Unknown option in option IAADDR( optType="
                 << code << "). Option ignored." << LogEnd;
        };
        pos += length;
    }
}

TSrvOptIAAddress::TSrvOptIAAddress(SPtr<TIPv6Addr> addr, unsigned long pref,
                                   unsigned long valid, TMsg* parent)
    :TOptIAAddress(addr,pref,valid, parent) {

}

bool TSrvOptIAAddress::doDuties() {
    return true;
}
