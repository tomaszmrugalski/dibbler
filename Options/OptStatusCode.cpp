#include <stdlib.h>
#include <string>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
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
    delete Message;
}


 int TOptStatusCode::getSize()
{
    return this->Message.length()+6;
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
	clog << logger::logNotice << "Status SUCCESS :" << Message << logger::endl;
	break;
    case STATUSCODE_UNSPECFAIL:
	clog << logger::logNotice << "Status UNSPECIFIED FAILURE :" << Message << logger::endl;
	break;
    case STATUSCODE_NOADDRSAVAIL:
	clog << logger::logNotice << "Status NO ADDRS AVAILABLE :" << Message << logger::endl;
	break;
    case STATUSCODE_NOBINDING:
	clog << logger::logNotice << "Status NO BINDING:" << Message << logger::endl;
	break;
    case STATUSCODE_NOTONLINK:
	clog << logger::logNotice << "Status NOT ON LINK:" << Message << logger::endl;
	break;
    case STATUSCODE_USEMULTICAST:
	clog << logger::logNotice << "Status USE MULTICAST:" << Message << logger::endl;
	break;
    }
    return true;
}
