#include <stdlib.h>
#include <iostream>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "OptTimeZone.h"
#include "DHCPConst.h"


using namespace std;

TOptTimeZone::TOptTimeZone(string timeZone, TMsg* parent)
    :TOpt(OPTION_TIME_ZONE, parent), Zone(timeZone)
{
  
}

TOptTimeZone::TOptTimeZone(char *&buf, int &bufsize,TMsg* parent)
    :TOpt(OPTION_TIME_ZONE, parent), Zone("")
{
   char* str=new char[bufsize+1];
   memcpy(str,buf,bufsize);
   str[bufsize]=0;
   Zone=TTimeZone(str);
   delete str;
   bufsize-=Zone.getLength();
   buf+=Zone.getLength();
}

char * TOptTimeZone::storeSelf(char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;    
    memcpy(buf,Zone.get().c_str(),Zone.getLength());
    return buf+Zone.getLength();
}

int TOptTimeZone::getSize()
{
    return Zone.getLength()+4;
}

string TOptTimeZone::getTimeZone()
{
    return Zone.get();
}

void TOptTimeZone::setTimeZone(string timeZone)
{
    Zone=TTimeZone(timeZone);
}

bool TOptTimeZone::isValid()
{
    return Zone.isValid();
}
