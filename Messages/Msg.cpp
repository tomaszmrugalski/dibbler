#include <stdlib.h>
#include <cmath>
#ifdef WIN32
#include <winsock2.h>
#endif
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"

#include "Msg.h"
#include "Opt.h"
#include "Logger.h"

TMsg::TMsg(int iface, SmartPtr<TIPv6Addr> addr, char* &buf, int &bufSize)
{
    unsigned char * buf2 = (unsigned char *)(buf+1);
    setAttribs(iface, addr, 0, 0);
    this->MsgType=buf[0];
    this->TransID= ((long)buf2[0])<<16 | ((long)buf2[1])<<8 | (long)buf2[2];
    buf+=4; bufSize-=4;
}

TMsg::TMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType)
{
    long tmp = rand() % (255*255*255);
    setAttribs(iface,addr,msgType,tmp);
}

TMsg::TMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType,  long transID)
{
    setAttribs(iface,addr,msgType,transID);
}

void TMsg::setAttribs(int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID)
{
/*    if (addr) {
        this->PeerAddr=new char[16];
    	memcpy(this->PeerAddr,addr,16);
    }  else 
        this->PeerAddr=NULL;*/
    PeerAddr=addr;

    this->Iface=iface;
    TransID=transID;
    IsDone=false;
    MsgType=msgType;
    this->pkt=NULL;
}

int TMsg::getSize()
{
    SmartPtr<TOpt> Option;
    int pktsize=0;
    Options.first();
    while( Option = Options.get() )
	pktsize+=Option->getSize();
    return pktsize+4;
}

unsigned long TMsg::getTimeout()
{
	return 0;
}

long TMsg::getType()
{
    return MsgType;
}

long TMsg::getTransID()
{
    return TransID;
}

TContainer< SmartPtr<TOpt> > TMsg::getOptLst()
{
    return Options;
}

bool TMsg::check()
{
	return false;
}

void TMsg::send()
{
    //FIXME:what the hell means this is not initialized pointer
    char *tmppkt = this->pkt;

    *(tmppkt++) = (char)MsgType;
    *(tmppkt++) = * ( ( (char*)&TransID ) + 2);
    *(tmppkt++) = * ( ( (char*)&TransID ) + 1);
    *(tmppkt++) = * ( ( (char*)&TransID ) + 0);
    Options.first();
    SmartPtr<TOpt> Option;
    while( Option = Options.get() )
    {
        Option->storeSelf(tmppkt);
        tmppkt += Option->getSize();
    }


    // FIXME: So we have constructed a message. Should we send it?
}

SmartPtr<TOpt> TMsg::getOption(int type)
{
	SmartPtr<TOpt> Option;
	
	Options.first();
	while ( Option = Options.get() ) 
	{
		if (Option->getOptType()==type) 
			return Option;
	}
	return SmartPtr<TOpt>();
}

void TMsg::firstOption()
{
    Options.first();
}

int TMsg::countOption()
{
    return Options.count();
}

SmartPtr<TOpt> TMsg::getOption()
{
    return Options.get();
}

TMsg::~TMsg()
{
}

SmartPtr<TIPv6Addr> TMsg::getAddr()
{
	return PeerAddr;
}

int TMsg::getIface()
{
	return this->Iface;
}

bool TMsg::isDone()
{
    return IsDone;
}
