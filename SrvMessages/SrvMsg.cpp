#ifdef WIN32
#include <winsock2.h>
#endif

#ifdef LINUX
#include <netinet/in.h>
#endif

#include "SrvMsg.h"
#include "SrvTransMgr.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptPreference.h"
#include "SrvOptElapsed.h"
#include "SrvOptServerUnicast.h"
#include "SrvOptStatusCode.h"
#include "SrvOptRapidCommit.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "SrvOptDomainName.h"
#include "SrvOptTimeZone.h"
#include "Logger.h"

#include "SrvIfaceMgr.h"
#include "AddrClient.h"

//Constructor builds message on the basis of received message
TSrvMsg::TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                 SmartPtr<TSrvTransMgr> TransMgr, 
                 SmartPtr<TSrvCfgMgr> CfgMgr,
                 SmartPtr<TSrvAddrMgr> AddrMgr,
                 int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID)
                 :TMsg(iface, addr, msgType, transID)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
}

//Constructor builds message on the basis of buffer
TSrvMsg::TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                 SmartPtr<TSrvTransMgr> TransMgr, 
                 SmartPtr<TSrvCfgMgr> CfgMgr,
                 SmartPtr<TSrvAddrMgr> AddrMgr,
                 int iface,  SmartPtr<TIPv6Addr> addr,
                 char* buf,  int bufSize)
                 :TMsg(iface, addr, buf, bufSize)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);

    //After reading meessage code and transaction id	
    //read options contained in message    
    int pos=0;
    while (pos<bufSize)		 
    {
        short code = ntohs( * ((short*) (buf+pos)));
        pos+=2;
        short length = ntohs(*((short*)(buf+pos)));
        pos+=2;
        SmartPtr<TOpt> ptr;
        if ((code>0)&&(code<=24) || (code==OPTION_DNS_RESOLVERS) ||
            (code==OPTION_DOMAIN_LIST) || (code==OPTION_NTP_SERVERS) || (code==OPTION_TIME_ZONE))
        {                
            if((canBeOptInMsg(MsgType,code))&&
                (canBeOptInOpt(MsgType,0,code)))
            {
                ptr= SmartPtr<TOpt>(); // NULL
                switch (code)
                {
                case OPTION_CLIENTID:
                    ptr = new TSrvOptClientIdentifier(buf+pos,length,this);
                    break;
                case OPTION_SERVERID:
                    ptr =new TSrvOptServerIdentifier(buf+pos,length,this);
                    break;
                case OPTION_IA:
                    ptr = new TSrvOptIA_NA(buf+pos,length,this);
                    break;
                case OPTION_ORO:
                    ptr = new TSrvOptOptionRequest(buf+pos,length,this);
                    break;
                case OPTION_PREFERENCE:
                    ptr = new TSrvOptPreference(buf+pos,length,this);
                    break;
                case OPTION_ELAPSED_TIME:
                    ptr = new TSrvOptElapsed(buf+pos,length,this);
                    break;
                case OPTION_UNICAST:
                    ptr = new TSrvOptServerUnicast(buf+pos,length,this);
                    break;
                case OPTION_STATUS_CODE:
                    ptr = new TSrvOptStatusCode(buf+pos,length,this);
                    break;
                case OPTION_RAPID_COMMIT:
                    ptr = new TSrvOptRapidCommit(buf+pos,length,this);
                    break;
                case OPTION_DNS_RESOLVERS:
                    ptr = new TSrvOptDNSServers(buf+pos,length,this);
                    break;
                case OPTION_NTP_SERVERS:
                    ptr = new TSrvOptNTPServers(buf+pos,length,this);
                    break;
                case OPTION_DOMAIN_LIST:
                    ptr = new TSrvOptDomainName(buf+pos, length,this);
                    break;
                case OPTION_TIME_ZONE:
                    ptr = new TSrvOptTimeZone(buf+pos, length,this);
                    break;
                case OPTION_IA_TA:
                case OPTION_RECONF_ACCEPT:
                case OPTION_USER_CLASS:
                case OPTION_VENDOR_CLASS:
                case OPTION_VENDOR_OPTS:
                case OPTION_RECONF_MSG:
                case OPTION_AUTH_MSG:
                case OPTION_RELAY_MSG:
                    clog<< logger::logWarning <<"Option nr:" << code<< "not supported "
                        <<" in field of message (type="<< MsgType <<") in this version of server."<<logger::endl;
                    break;
                }
                if ((ptr)&&(ptr->isValid()))
                    Options.append( ptr );
                else
                    clog<<logger::logWarning<<"Option nr:"<<code <<" invalid. Option ignored" << logger::endl;
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

}

TSrvMsg::TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                 SmartPtr<TSrvTransMgr> TransMgr, 
                 SmartPtr<TSrvCfgMgr> CfgMgr,
                 SmartPtr<TSrvAddrMgr> AddrMgr,
                 int iface, 
                 SmartPtr<TIPv6Addr> addr,  int msgType)
                 :TMsg(iface,addr,msgType)
{
    setAttribs(IfaceMgr,TransMgr,CfgMgr,AddrMgr);
}

void TSrvMsg::setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                         SmartPtr<TSrvTransMgr> TransMgr, 
                         SmartPtr<TSrvCfgMgr> CfgMgr,
                         SmartPtr<TSrvAddrMgr> AddrMgr)
{
    SrvTransMgr=TransMgr;	
    SrvIfaceMgr=IfaceMgr;	
    SrvCfgMgr=CfgMgr;
    SrvAddrMgr=AddrMgr;
    FirstTimeStamp = now();
    this->MRT=0;

    /*    			
    LastTimeStamp  = now();			

    RC = 0;
    RT = 0;
    IRT = 0;
    MRT = 0;
    MRC = 0;
    MRD = 0;*/
}

void TSrvMsg::answer(SmartPtr<TMsg> answer)
{
    SmartPtr<TSrvOptClientIdentifier> srvDUID;
    srvDUID=(Ptr*)this->getOption(OPTION_CLIENTID);

    SmartPtr<TSrvOptClientIdentifier> clntDUID;
    clntDUID=(Ptr*)answer->getOption(OPTION_CLIENTID);
    if (!( srvDUID->getDUID()==clntDUID->getDUID() ))
        return;
    //retransmission
    TMsg::send();
    this->SrvIfaceMgr->sendMulticast(this->Iface,(char*)this->pkt,
        this->getSize(),this->PeerAddr);
    return;
}

void TSrvMsg::doDuties()
{
    if ( (this->FirstTimeStamp+this->MRT) >= now() )
        this->IsDone = true;
}

unsigned long TSrvMsg::getTimeout()
{
    if (this->FirstTimeStamp+this->MRT - now() > 0 )
        return this->FirstTimeStamp+this->MRT - now(); 
    else
        return 0;
}

void TSrvMsg::send()
{
    std::clog << logger::logNotice << "Sending msg:type=" << this->getType() 
        << ", size=" <<this->getSize() 
        << hex << ",transID=" << this->getTransID() << dec 
        << ", " << this->countOption() << " opts:";
    SmartPtr<TOpt> ptrOpt;
    this->firstOption();
    while (ptrOpt = this->getOption() )
        std::clog << " " << ptrOpt->getOptType() << "(" << ptrOpt->getSize() << ")";
    std::clog << logger::endl;
    TMsg::send();
    // FIXME: If server supports unicast, sendUnicast...
    this->SrvIfaceMgr->sendMulticast(this->Iface, this->pkt, this->getSize(), this->PeerAddr);
}

//void TSrvMsg::getFreeAddressesForClient (SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > &clntAllClasses,
//					 SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > &clntClasses,
//					 long* &clntFreeAddr, long &totalFreeAddresses, 
//					 SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, int iface, bool rapid)
//{
//
//    clntAllClasses=SrvCfgMgr->getClassesForClient(duid,addr,iface, rapid);
//
//    totalFreeAddresses=0;
//    clntFreeAddr=NULL;    //FIXME:remember to free
//    long *classAssignAddr=NULL;
//    long *clntAssignAddr=NULL; 
//    long count = clntAllClasses->count();
//
//    clntClasses=new TContainer<SmartPtr<TSrvCfgAddrClass> > ();
//
//    if(count)
//    {
//        classAssignAddr=new long[count];
//        clntAssignAddr=new long[count];
//        clntFreeAddr=new long[count];
//        memset(clntFreeAddr,0,count*sizeof(long));
//
//        SrvAddrMgr->getAddrsCount(clntAllClasses, clntAssignAddr, classAssignAddr, 
//            duid, iface);
//        int i=0;
//        int j=0;
//        SmartPtr<TSrvCfgAddrClass> ptrClass;
//        clntAllClasses->first();
//        while(ptrClass=clntAllClasses->get())
//        {
//            long clntAvail=ptrClass->getMaxClientLease()-clntAssignAddr[i];
//            long allAvail=ptrClass->getMaxLease()-classAssignAddr[i];
//            if (clntAvail>allAvail)
//                clntFreeAddr[j]=allAvail;
//            else
//                clntFreeAddr[j]=clntAvail;
//            if (clntFreeAddr[j])
//            {
//                clntClasses->append(ptrClass);
//                totalFreeAddresses+=clntFreeAddr[j];
//                j++;
//            }
//            i++;
//        }
//        delete[] classAssignAddr;
//        delete[] clntAssignAddr;
//	//delete[] clntFreeAddr;
//    }
//    else
//        clntClasses=clntAllClasses;
//}

bool TSrvMsg::appendRequestedOptions(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, 
        int iface, SmartPtr<TSrvOptOptionRequest> reqOpts)
{
    bool newOptionAssigned = false;
    if ((reqOpts->getOptCnt())&&(SrvCfgMgr->isClntSupported(duid,addr,iface)))
    {
        SmartPtr<TSrvCfgIface>  ptrIface=SrvCfgMgr->getIfaceByID(iface);
    
        if ((reqOpts->isOption(OPTION_DNS_RESOLVERS))&&ptrIface->getDNSSrvLst().count())
        {
            SmartPtr<TSrvOptDNSServers> dnsLst=
                new TSrvOptDNSServers(ptrIface->getDNSSrvLst(),this);
            Options.append((Ptr*)dnsLst);
            newOptionAssigned = true;
        };
        
        if ((reqOpts->isOption(OPTION_NTP_SERVERS))&&ptrIface->getNTPSrvLst().count())
        {
            SmartPtr<TSrvOptNTPServers> ntpLst=
                new TSrvOptNTPServers(ptrIface->getNTPSrvLst(),this);
            Options.append((Ptr*)ntpLst);
            newOptionAssigned = true;
        };

        if ((reqOpts->isOption(OPTION_TIME_ZONE))&&(ptrIface->getTimeZone()!=""))
        {
            SmartPtr<TSrvOptTimeZone> timeZone=
                new TSrvOptTimeZone(ptrIface->getTimeZone(),this);
            Options.append((Ptr*)timeZone);
            newOptionAssigned = true;
        };

        if ((reqOpts->isOption(OPTION_DOMAIN_LIST))&&(ptrIface->getDomain()!=""))
        {
            SmartPtr<TSrvOptDomainName> domain=
                new TSrvOptDomainName(ptrIface->getDomain(),this);
            Options.append((Ptr*)domain);
            newOptionAssigned = true;
        };
    }
    return newOptionAssigned;
}
