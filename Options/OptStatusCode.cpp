/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <string.h>
#include <string>
#include "Portable.h"
#include "OptStatusCode.h"
#include "Logger.h"

#if defined(LINUX) || defined(BSD)
#include <arpa/inet.h>
#endif

using namespace std;

TOptStatusCode::TOptStatusCode(const char * buf, size_t  len, TMsg* parent)
    :TOpt(OPTION_STATUS_CODE, parent), Valid_(false)
{
    if (len < 2) {
        buf += len;
        len = 0;
        return;
    }
    StatusCode_ = readUint16(buf);
    buf += sizeof(uint16_t);
    len -= sizeof(uint16_t);
    char *msg = new char[len+1];
    memcpy(msg, buf, len);
    msg[len]=0;
    Message_ = (string)msg;
    delete [] msg;
    Valid_ = true;
}


size_t TOptStatusCode::getSize() {
    return Message_.length() + 6;
}

int TOptStatusCode::getCode() {
    return StatusCode_;
}

string TOptStatusCode::getText() {
    return Message_;
}


char * TOptStatusCode::storeSelf( char* buf)
{
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    buf = writeUint16(buf, StatusCode_);
    strncpy((char *)buf, Message_.c_str(), Message_.length());
    buf += Message_.length();
    return buf;
}
TOptStatusCode::TOptStatusCode(int status,const std::string& message, TMsg* parent)
    :TOpt(OPTION_STATUS_CODE, parent), Message_(message), StatusCode_(status),
     Valid_(true) {
}

bool TOptStatusCode::doDuties()
{
    switch (StatusCode_) {
    case STATUSCODE_SUCCESS:
        Log(Notice) << "Status SUCCESS :" << Message_ << LogEnd;
        break;
    case STATUSCODE_UNSPECFAIL:
        Log(Notice) << "Status UNSPECIFIED FAILURE :" << Message_ << LogEnd;
        break;
    case STATUSCODE_NOADDRSAVAIL:
        Log(Notice) << "Status NO ADDRS AVAILABLE :" << Message_ << LogEnd;
        break;
    case STATUSCODE_NOBINDING:
        Log(Notice) << "Status NO BINDING:" << Message_ << LogEnd;
        break;
    case STATUSCODE_NOTONLINK:
        Log(Notice) << "Status NOT ON LINK:" << Message_ << LogEnd;
        break;
    case STATUSCODE_USEMULTICAST:
        Log(Notice) << "Status USE MULTICAST:" << Message_ << LogEnd;
        break;
    case STATUSCODE_NOPREFIXAVAIL:
        Log(Notice) << "Status NO PREFIX AVAILABLE :" << Message_ << LogEnd;
        break;
    }
    return true;
}
