/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptNTPServers.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "OptNTPServers.h"
#include "DHCPConst.h"

TOptNTPServers::TOptNTPServers(TContainer<SmartPtr<TIPv6Addr> > NTPSrvLst, TMsg* parent)
    :TOpt(OPTION_NTP_SERVERS, parent), NTPSrv(NTPSrvLst)
{
    
}

TOptNTPServers::TOptNTPServers(char* &buf,int &bufSize, TMsg* parent)
    :TOpt(OPTION_NTP_SERVERS, parent)
{
    while(bufSize>0)
    {
        this->NTPSrv.append(new TIPv6Addr(buf));
        bufSize-=16;
        buf+=16;
    }
    Valid=!bufSize;
}

char * TOptNTPServers::storeSelf(char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;
    if (NTPSrv.count())
    {
        SmartPtr<TIPv6Addr> addr;
        NTPSrv.first();
        while(addr=NTPSrv.get())
            buf=addr->storeSelf(buf);
    }
    else
        return buf;
    return buf;
}

int TOptNTPServers::getSize()
{
    if (NTPSrv.count())
        return 4+16*NTPSrv.count();
    else
        return 0;
}

void TOptNTPServers::firstNTPSrv()
{
    this->NTPSrv.first();
}

SmartPtr<TIPv6Addr> TOptNTPServers::getNTPSrv()
{
    return this->NTPSrv.get();
}

bool TOptNTPServers::isValid()
{
    return this->Valid;
}
