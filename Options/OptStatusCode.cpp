/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptStatusCode.cpp,v 1.7 2007-08-26 10:26:19 thomson Exp $
 *
 */

#include <string.h>
#include <string>
#include "Portable.h"
#include "OptStatusCode.h"
#include "Logger.h"

TOptStatusCode::TOptStatusCode( char * &buf, int  &len, TMsg* parent)
	:TOpt(OPTION_STATUS_CODE, parent)
{
    Valid=true;
    if (len<2)
    {
        Valid=false;
        buf+=len;
        len=0;
        return;
    }
    this->StatusCode = ntohs(*(short*)buf);
    buf +=2; len -=2;
    char *Message = new char[len+1];
    memcpy(Message,buf,len);
    Message[len]=0;
	this->Message=(string)Message;
    delete [] Message;
}


 int TOptStatusCode::getSize()
{
    return (int)(this->Message.length()+6);
}

 int TOptStatusCode::getCode()
{
    return StatusCode;
}

string TOptStatusCode::getText()
{
    return Message;
}


char * TOptStatusCode::storeSelf( char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;
    *(short*)buf = htons(this->StatusCode);
    buf+=2;
    strncpy((char *)buf,Message.c_str(),Message.length());
    buf+=Message.length();
    return buf;
}
TOptStatusCode::TOptStatusCode(int status,string message, TMsg* parent)
	:TOpt(OPTION_STATUS_CODE, parent)
{
    this->StatusCode = status;
    this->Message = message;
}

bool TOptStatusCode::doDuties()
{
    switch (StatusCode) {
    case STATUSCODE_SUCCESS:
	Log(Notice) << "Status SUCCESS :" << Message << LogEnd;
	break;
    case STATUSCODE_UNSPECFAIL:
	Log(Notice) << "Status UNSPECIFIED FAILURE :" << Message << LogEnd;
	break;
    case STATUSCODE_NOADDRSAVAIL:
	Log(Notice) << "Status NO ADDRS AVAILABLE :" << Message << LogEnd;
	break;
    case STATUSCODE_NOBINDING:
	Log(Notice) << "Status NO BINDING:" << Message << LogEnd;
	break;
    case STATUSCODE_NOTONLINK:
	Log(Notice) << "Status NOT ON LINK:" << Message << LogEnd;
	break;
    case STATUSCODE_USEMULTICAST:
	Log(Notice) << "Status USE MULTICAST:" << Message << LogEnd;
	break;
    case STATUSCODE_NOPREFIXAVAIL:
	Log(Notice) << "Status NO PREFIX AVAILABLE :" << Message << LogEnd;
	break;
    }
    return true;
}
