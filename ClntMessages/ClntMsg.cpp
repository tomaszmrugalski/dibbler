/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsg.cpp,v 1.4 2004-09-27 22:01:01 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.2  2004/06/04 16:55:27  thomson
 * *** empty log message ***
 *
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
    SmartPtr<TOpt> ptr;
    while (pos<bufSize) {
	ptr = 0;
        short code = ntohs( *((short*) (buf+pos)));
        pos+=2;
        short length = ntohs(*((short*) (buf+pos)));
        pos+=2;

	if (!canBeOptInMsg(MsgType,code)) {
            Log(Warning) << "Option " << code << " is not allowed in " << MsgType
			 << " message. Ignoring." << LogEnd;
	    pos+=length;
	    continue;
	}
	if (!canBeOptInOpt(MsgType,0,code)) {
	    Log(Warning) << "Option " << code << " is not allowed directly in " << MsgType
			 << " message. Ignoring." << LogEnd;
	    pos+=length;
	    continue;
	}
	switch (code) {
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
	    Log(Warning) << "Option " << code<< "not supported in message " 
			 << MsgType << ") in this version of client." << LogEnd;
	    break;
	}
	
	if ( (ptr) && (ptr->isValid())) {
                    Options.append( ptr );
	} else {
	    Log(Warning) << "Option " << ptr->getOptType() << " is invalid. Ignoring." << LogEnd;
	}
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
    if (!RC)
        RT=(int)floor(0.5+IRT+IRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    else
	RT =(int) floor(0.5+2.0*RT+RT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    if (MRT != 0 && RT>MRT) 
	RT = (int) floor(0.5+MRT + MRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
    
    if ((MRD != 0) && (RT>MRD))
        RT = MRD;
    if (MRD) MRD-=RT;
    
    RC++;
    
    TMsg::send();

    SmartPtr<TIfaceIface> ptrIface = ClntIfaceMgr->getIfaceByID(Iface);
    if (PeerAddr) {
	Log(Debug) << "Sending " << this->getName() << " on /" << ptrIface->getName() 
		   << "/" << Iface << " to unicast addr " << *PeerAddr << "." << LogEnd;
	ClntIfaceMgr->sendUnicast(Iface,pkt,getSize(),PeerAddr);
    } else {
	Log(Debug) << "Sending " << this->getName() << " on /" << ptrIface->getName() 
		   << "/" << Iface << " to mulitcast." << LogEnd;
	ClntIfaceMgr->sendMulticast(Iface, pkt, getSize());
    }
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

