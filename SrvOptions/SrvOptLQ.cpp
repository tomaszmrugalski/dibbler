/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */


#include "Msg.h"
#include "Logger.h"
#include "SrvOptLQ.h"
#include "SrvOptIAAddress.h"
#include "OptDUID.h"
#include "Portable.h"

// --- TSrvOptLQ ---
TSrvOptLQ::TSrvOptLQ(char * buf, int bufsize, TMsg* parent)
    :TOpt(OPTION_LQ_QUERY, parent)
{
    IsValid = true;

    // @TODO parse
    if (bufsize<17) {
        Log(Warning) << "Truncated (len=" << bufsize << ", at least 17 required) option OPTION_LQ_QUERY received." << LogEnd;
        IsValid = false;
        return;
    }
    QueryType = static_cast<ELeaseQueryType>(buf[0]);
    Addr = new TIPv6Addr(buf+1);
    int pos = 17;

    while (pos<bufsize) {
        if (pos+4>bufsize) {
            IsValid = false;
            Log(Warning) << "Truncated IA_NA option received." << LogEnd;
            return;
        }
        int code=buf[pos]*256+buf[pos+1];
        pos+=2;
        int length=buf[pos]*256+buf[pos+1];
        pos+=2;

        if (allowOptInOpt(parent->getType(), OPTION_LQ_QUERY, code)) {
            switch (code) {
            case OPTION_IAADDR:
                SubOptions.append( new TSrvOptIAAddress(buf+pos, length, Parent));
                break;
            case OPTION_CLIENTID:
                SubOptions.append( new TOptDUID(OPTION_CLIENTID, buf+pos, length, Parent) );
                break;
            default:
                Log(Warning) << "Not supported option " << code << " received in LQ_QUERY option." << LogEnd;
            }
        } else {
            Log(Warning) << "Illegal option " << code << " received inside LQ_QUERY option." << LogEnd;
        }

        pos += length;
        continue;
    }

}

// --- TSrvOptBLQ ---
TSrvOptLQ::TSrvOptLQ(char *buf, int bufSize, TMsg *parent, int msgType)
    :TOpt(OPTION_LQ_QUERY, parent)
{
    int pos=0, prvOpt=0;
    unsigned short code;
    unsigned short length;

    if (bufSize<17) {
        Log(Warning) << "Truncated (len=" << bufSize << ", at least 17 required) option OPTION_LQ_QUERY received." << LogEnd;
        IsValid = false;
        return;
    }

    unsigned short tmpl=0;
    int pos2=0;
    for(pos2=0;pos2<bufSize;pos2++) {
        tmpl = buf[pos2];
        Log(Debug) << "pos"<<pos2<<":"<<tmpl <<LogEnd;
        tmpl=0;
    }
    while (pos<bufSize)	{

        code   = buf[pos]*256 + buf[pos+1];
        pos+=2;
        length = buf[pos]*256 + buf[pos+1];
        pos+=2;
        if (pos+length>bufSize) {
            Log(Error) << "Malformed option (type=" << code << ", len=" << length
                       << ", offset from beginning of DHCPv6 message." << pos+6 /* 6=msgSize+msgType+trans-id */
                       << "Message dropped." << LogEnd;
            return;
        }

        SPtr<TOpt> ptr;

        if (!allowOptInOptInBulk(msgType,prvOpt,code,pos)) {
            Log(Warning) <<"Option " << code << " can't be present in message (type="
                         << msgType <<") directly. Option ignored." << LogEnd;
            pos+=length;
            continue;
        }

        switch (code) {
        case OPTION_CLIENTID:
            SubOptions.append( new TOptDUID(OPTION_CLIENTID, buf+pos, length, Parent));
            prvOpt = code;
            break;
        case OPTION_REMOTE_ID:
            //SubOptions.append(new TOptVendorSpecInfo(code, buf+pos, length, this));
            break;
        case OPTION_RELAY_ID:
            SubOptions.append( new TSrvOptIAAddress(buf+pos, length, parent));
            break;
        case OPTION_IAADDR:
            SubOptions.append( new TSrvOptIAAddress(buf+pos, length, parent));
            break;
        case OPTION_LQ_QUERY:
            SubOptions.append(this);
            break;
        default:
            Log(Warning) << "Option type " << code << " not supported yet." << LogEnd;
            break;
        };


        pos += length;
    }
   // Log(Debug) << "SubOptCount:" << this->countOption() <<LogEnd;
}

bool TSrvOptLQ::doDuties() {
    return true;
}

ELeaseQueryType TSrvOptLQ::getQueryType() {
    return QueryType;
}

SPtr<TIPv6Addr> TSrvOptLQ::getLinkAddr() {
    return Addr;
}

size_t TSrvOptLQ::getSize() {
    SPtr<TOpt> opt;
    int len = 17;
    SubOptions.first();
    while (opt = SubOptions.get() ) {
        len += opt->getSize();
    }
    return len;
}

char * TSrvOptLQ::storeSelf(char* buf)
{
    Log(Error) << "LQ: Something is wrong. Server was trying to send OPTION_LQ_QUERY option." << LogEnd;
    return buf;
}


// -----------------------------------------------------------------------------------

TSrvOptLQClientData::TSrvOptLQClientData(TMsg * parent)
    :TOpt(OPTION_CLIENT_DATA, parent)
{
}

size_t TSrvOptLQClientData::getSize()
{
    int cnt = 0;
    SPtr<TOpt> x;
    SubOptions.first();
    while ( x=SubOptions.get() ) {
        cnt += x->getSize();
    }
    return cnt+4;
}

char* TSrvOptLQClientData::storeSelf(char* buf)
{
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    buf = storeSubOpt(buf);
    return buf;
}

bool TSrvOptLQClientData::doDuties()
{
    return true;
}

// -----------------------------------------------------------------------------------

TSrvOptLQClientTime::TSrvOptLQClientTime(unsigned int value, TMsg* parent)
    :TOptInteger(OPTION_CLT_TIME, 4, value, parent)
{

}

bool TSrvOptLQClientTime::doDuties()
{
    return true;
}

// -----------------------------------------------------------------------------------

TSrvOptLQRelayData::TSrvOptLQRelayData(SPtr<TIPv6Addr> addr, TMsg* parent)
    :TOptGeneric(OPTION_LQ_RELAY_DATA, parent)
{
    // @TODO - implement this
}

// -----------------------------------------------------------------------------------

TSrvOptLQClientLink::TSrvOptLQClientLink(List(TIPv6Addr) AddrLst, TMsg * parent)
    :TOpt(OPTION_LQ_CLIENT_LINK, parent)
{
    LinkAddrLst = AddrLst;
}
