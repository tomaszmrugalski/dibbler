/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsg.cpp,v 1.2 2004-06-04 16:55:27 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef LINUX
#include <netinet/in.h>
#endif

#include <cmath>


#include "ClntMsg.h"
#include "ClntTransMgr.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptIA_NA.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptPreference.h"
#include "ClntOptElapsed.h"
#include "ClntOptServerUnicast.h"
#include "ClntOptStatusCode.h"
#include "ClntOptRapidCommit.h"
#include "ClntOptDNSServers.h"
#include "ClntOptNTPServers.h"
#include "ClntOptTimeZone.h"
#include "ClntOptDomainName.h"
#include "Logger.h"

//Constructor builds message on the basis of buffer
TClntMsg::TClntMsg(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                   SmartPtr<TClntTransMgr> TransMgr, 
                   SmartPtr<TClntCfgMgr> CfgMgr,
                   SmartPtr<TClntAddrMgr> AddrMgr,
                   int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize)
                   :TMsg(iface, addr, buf, bufSize)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
    //After reading message code and transactionID   
    //read options contained in message    
    int pos=0;
    while (pos<bufSize)		 
    {
        short code = ntohs( *((short*) (buf+pos)));
        pos+=2;
        short length = ntohs(*((short*) (buf+pos)));
        pos+=2;
        SmartPtr<TOpt> ptr;
        if (((code>0)&&(code<=24)) || (code==OPTION_DNS_RESOLVERS)||
            (code==OPTION_DOMAIN_LIST) || (code==OPTION_NTP_SERVERS) || (code==OPTION_TIME_ZONE))
        {                
            if((canBeOptInMsg(MsgType,code))&&
                (canBeOptInOpt(MsgType,0,code)))
            {
                switch (code)
                {
                case OPTION_CLIENTID:
                    ptr = new TClntOptClientIdentifier(buf+pos,length,this);
                    break;
                case OPTION_SERVERID:
                    ptr =new TClntOptServerIdentifier(buf+pos,length,this);
                    break;
                case OPTION_IA:
                    ptr = new TClntOptIA_NA(buf+pos,length,this);
                    break;
                case OPTION_ORO:
                    ptr = new TClntOptOptionRequest(buf+pos,length,this);
                    break;
                case OPTION_PREFERENCE:
                    ptr = new TClntOptPreference(buf+pos,length,this);
                    break;
                case OPTION_ELAPSED_TIME:
                    ptr = new TClntOptElapsed(buf+pos,length,this);
                    break;
                case OPTION_UNICAST:
                    ptr = new TClntOptServerUnicast(buf+pos,length,this);
                    break;
                case OPTION_STATUS_CODE:
                    ptr = new TClntOptStatusCode(buf+pos,length,this);
                    break;
                case OPTION_RAPID_COMMIT:
                    ptr = new TClntOptRapidCommit(buf+pos,length,this);
                    break;
                case OPTION_DNS_RESOLVERS:
                    ptr = new TClntOptDNSServers(buf+pos,length,this);
                    break;
                case OPTION_NTP_SERVERS:
                    ptr = new TClntOptNTPServers(buf+pos,length,this);
                    break;
                case OPTION_DOMAIN_LIST:
                    ptr = new TClntOptDomainName(buf+pos, length, this);
                    break;
                case OPTION_TIME_ZONE:
                    ptr = new TClntOptTimeZone(buf+pos, length,this);
                    break;

                case OPTION_IA_TA:
                case OPTION_RECONF_ACCEPT:
                case OPTION_USER_CLASS:
                case OPTION_VENDOR_CLASS:
                case OPTION_VENDOR_OPTS:
                case OPTION_RECONF_MSG:
                case OPTION_RELAY_MSG:
                case OPTION_AUTH_MSG:
                case OPTION_INTERFACE_ID:
                    clog<< logger::logWarning <<"Option opttype=" << code<< "not supported "
                        <<" in field of message (type="<< MsgType <<") in this version of client."<<logger::endl;
                    break;
                }
                if ((ptr)&&(ptr->isValid()))
                    Options.append( ptr );
                else
                    std::clog<<logger::logWarning<<"Option nr:"<<ptr->getOptType()
                        <<" invalid."<<"Option ignored."<<logger::endl;
            }
            else
                clog << logger::logWarning << "Illegal option received, opttype=" << code 
                    << " in field of message (type="<< MsgType <<")"<<logger::endl;
        }
        else
        {
                clog << logger::logWarning <<"Unknown option in package (" 
                    << code << ", addr = " << *addr << "). Option ignored." << logger::endl;
        };
        pos+=length;
    }

    SmartPtr<TClntOptServerIdentifier> ptrSrvID;
    if (ptrSrvID=(Ptr*)this->getOption(OPTION_SERVERID))
    {
        this->firstOption();
        while (SmartPtr<TOpt> ptrOpt=getOption())
        {
            switch (ptrOpt->getOptType())
            {
                case OPTION_DNS_RESOLVERS:
                    ((TClntOptDNSServers*)&(*ptrOpt))->setSrvDuid(ptrSrvID->getDUID());
                    break;
                case OPTION_DOMAIN_LIST:
                    ((TClntOptDomainName*)&(*ptrOpt))->setSrvDuid(ptrSrvID->getDUID());
                    break;
                case OPTION_NTP_SERVERS:
                    ((TClntOptNTPServers*)&(*ptrOpt))->setSrvDuid(ptrSrvID->getDUID());
                    break;
                case OPTION_TIME_ZONE:
                    ((TClntOptTimeZone*)&(*ptrOpt))->setSrvDuid(ptrSrvID->getDUID());
                    break;
            }
        }
    }
}

TClntMsg::TClntMsg(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                   SmartPtr<TClntTransMgr> TransMgr, 
                   SmartPtr<TClntCfgMgr> CfgMgr,
                   SmartPtr<TClntAddrMgr> AddrMgr,
                   int iface, 
                   SmartPtr<TIPv6Addr> addr, int msgType)
                   :TMsg(iface,addr,msgType)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
}

void TClntMsg::setAttribs(SmartPtr<TClntIfaceMgr> IfaceMgr, 
                          SmartPtr<TClntTransMgr> TransMgr, 
                          SmartPtr<TClntCfgMgr> CfgMgr,
                          SmartPtr<TClntAddrMgr> AddrMgr)
{
    ClntTransMgr=TransMgr;	
    ClntIfaceMgr=IfaceMgr;	
    ClntCfgMgr=CfgMgr;
    ClntAddrMgr=AddrMgr;

    FirstTimeStamp = now();			
    LastTimeStamp  = now();			

    RC = 0;
    RT = 0;
    IRT = 0;
    MRT = 0;
    MRC = 0;
    MRD = 0;
}

unsigned long TClntMsg::getTimeout()
{
    long diff=(LastTimeStamp+RT) - now();
    return (diff<0) ? 0 : diff;
}

void TClntMsg::send()
{
	//FIXME:Here was division by zero & what if RT=0
	//		it's not accrptable. (FIXED)
    if (!RC)
        RT=(int)floor(0.5+IRT+IRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    else
	    RT =(int) floor(0.5+2.0*RT+RT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

	//FIXME:rand() is supposed to return number in range: (-0.1,0.1)
	//FIXME:it's intresting even if it is above MRT, as it was set in prev instruction
	//		it can be set again below this value, so in SOLICIT message ending condition
	//		should be changed a little bit. By the way there is no SOLICIT ending condition
	//		this transaction can end only after receiving an answer

	if (MRT != 0 && RT>MRT) 
		RT = (int) floor(0.5+MRT + MRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    
    if ((MRD != 0) && (RT>MRD))
        RT = MRD;
    if (MRD) MRD-=RT;
    
    RC++;
    	
	TMsg::send();
    
	if (PeerAddr)
		ClntIfaceMgr->sendUnicast(Iface,pkt,getSize(),PeerAddr);
	else
		ClntIfaceMgr->sendMulticast(Iface, pkt, getSize());
	LastTimeStamp = now();
}

SmartPtr<TClntTransMgr> TClntMsg::getClntTransMgr()
{
    return this->ClntTransMgr;
}

SmartPtr<TClntAddrMgr> TClntMsg::getClntAddrMgr()
{
    return this->ClntAddrMgr;
}

SmartPtr<TClntCfgMgr> TClntMsg::getClntCfgMgr()
{
    return this->ClntCfgMgr;
}

SmartPtr<TClntIfaceMgr> TClntMsg::getClntIfaceMgr()
{
    return this->ClntIfaceMgr;
}
