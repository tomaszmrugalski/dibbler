#include "ClntMsgRenew.h"
#include "DHCPConst.h"
#include "ClntOptIA_NA.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptOptionRequest.h"
#include "Logger.h"
#include "SmartPtr.h"
#include "ClntOptDNSServers.h"
#include "ClntOptNTPServers.h"
#include "ClntOptDomainName.h"
#include "ClntOptTimeZone.h"

#include <cmath>

TClntMsgRenew::TClntMsgRenew(SmartPtr<TClntIfaceMgr> IfaceMgr,
    SmartPtr<TClntTransMgr> TransMgr,
    SmartPtr<TClntCfgMgr> CfgMgr,
    SmartPtr<TClntAddrMgr> AddrMgr,
    TContainer<SmartPtr<TAddrIA> > ptrIALst)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,
        ptrIALst.getFirst()->getIface(),ptrIALst.getFirst()->getSrvAddr(),RENEW_MSG)
{
   // set transmission parameters
    IRT=REN_TIMEOUT;
    MRT=REN_MAX_RT;
    MRC=0;
    RT=0;

    // retransmit until T2 has been reached or any address has expired
    //it shhould be the same for all IAs
    MRD= ptrIALst.getFirst()->getT2Timeout();  
    
    // I don't think checking whether address expired is neccessary here
    // (address will be removed from iface by address manager automatically
    // Worth checking could be checking, whether valid time expired for all
    // addresses in any of IAs - in such a case (for all addresses should be 
    // valid<T2) and if it happens IA should be put in state NOTCONFIGURED
    // So MRD should be set to the value of T2 timeout
    // but RT in send method should be change in order to take into
    // consideration elapsing valid timeouts of addresses in IAs
    // in such a case doDuties 
    if (RT>MRD) 
        RT=MRD;

    // store our DUID
    Options.append(new TClntOptClientIdentifier(CfgMgr->getDUID(),this));

    // and say who's this message is for
    Options.append( new TClntOptServerIdentifier(ptrIALst.getFirst()->getDUID(),this));
    
    //Store all IAs to renew
    SmartPtr<TAddrIA> ptrAddrIA;
    ptrIALst.first();
    while(ptrAddrIA= ptrIALst.get())
          Options.append(new TClntOptIA_NA(ptrAddrIA,this));
    //Here should be addede option request option and other options, which
    //can be refreshed
    SmartPtr<TClntOptOptionRequest> ptrOptOptReq=
        new TClntOptOptionRequest(CfgMgr->getIface(ptrIALst.getFirst()->getIface()),this);
    Options.append((Ptr*)ptrOptOptReq);

    SmartPtr<TClntCfgIface> ptrIface;
    if (ptrIface=CfgMgr->getIface(ptrIALst.getFirst()->getIface()))
    {
        if (ptrIface->isReqDNSSrv()&&(ptrIface->getDNSSrvState()==CONFIGURED))
        {
            SmartPtr<TClntOptDNSServers> ptrDNSOpt=
                new TClntOptDNSServers(ptrIface->getProposedDNSSrv(),this);
            Options.append((Ptr*)ptrDNSOpt);
            // ptrIface->setDNSSrvState(INPROCESS);
        }

        if (ptrIface->isReqNTPSrv()&&(ptrIface->getNTPSrvState()==CONFIGURED))
        {
            SmartPtr<TClntOptNTPServers> ptrNTPOpt=
                new TClntOptNTPServers(ptrIface->getProposedNTPSrv(),this);
            Options.append((Ptr*)ptrNTPOpt);
            ptrIface->setNTPSrvState(INPROCESS);
        }
        
        if (ptrIface->isReqDomainName()&&(ptrIface->getDomainNameState()==CONFIGURED))
        {
            SmartPtr<TClntOptDomainName> ptrDomainOpt=
                new TClntOptDomainName(ptrIface->getProposedDomainName(),this);
            Options.append((Ptr*)ptrDomainOpt);
            //ptrIface->setDomainNameState(INPROCESS);
        }

        if (ptrIface->isReqTimeZone()&&(ptrIface->getTimeZoneState()==CONFIGURED))
        {
            SmartPtr<TClntOptTimeZone> ptrTimeOpt=
                new TClntOptTimeZone(ptrIface->getProposedTimeZone(),this);
            Options.append((Ptr*)ptrTimeOpt);
            ptrIface->setTimeZoneState(INPROCESS);
        }
    }
    pkt = new char[getSize()];
    this->IsDone = false;
    this->send();
}

/*TClntMsgRenew::TClntMsgRenew(SmartPtr<TClntIfaceMgr> IfaceMgr,
			     SmartPtr<TClntTransMgr> TransMgr,
			     SmartPtr<TClntCfgMgr> CfgMgr,
			     SmartPtr<TClntAddrMgr> AddrMgr,
			     SmartPtr<TAddrIA> ptrAddrIA)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,ptrAddrIA->getIface(),ptrAddrIA->getSrvAddr(),RENEW_MSG)
{
    // set transmission parameters
    IRT=REN_TIMEOUT;
    MRT=REN_MAX_RT;
    MRC=0;
    RT=(int)floor(0.5+IRT+IRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

    // retransmit until T2 has been reached or any address has expired
    MRD= ptrAddrIA->getT2Timeout();
    unsigned long valid = ptrAddrIA->getValidTimeout();
    if (MRD < valid)
    	MRD = valid;
    if (RT>MRD) 
        RT=MRD;

    // remember ptrIA in case we need it to send REBIND message...
    this->AddrIA = ptrAddrIA;

    // create OPTION_IA (with OPTION_IAADDR suboptions) 
    SmartPtr<TClntOptIA_NA> ptrIA =  new TClntOptIA_NA(ptrAddrIA,this);
    SmartPtr<TOpt> ptrOptIA = (Ptr*) ptrIA;
    Options.append(ptrOptIA);

    // store our DUID
    SmartPtr<TOpt> ptrSrvID = new TClntOptServerIdentifier(ptrAddrIA->getDUID(),this);
    Options.append( ptrSrvID);

    // and say who's this message is for
    SmartPtr<TOpt> ptrClntID = new TClntOptClientIdentifier(CfgMgr->getDUID(),this);
    Options.append(ptrClntID);

    pkt = new char[getSize()];
    this->send();
    this->IsDone = false;
}*/

void TClntMsgRenew::answer(SmartPtr<TMsg> Reply)
{
    SmartPtr<TOpt> opt;
    
    // get DUID
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) this->getOption(OPTION_SERVERID);
    
    SmartPtr<TClntOptOptionRequest> ptrOptionReqOpt=(Ptr*)getOption(OPTION_ORO);

    Reply->firstOption();
    // for each option in message... (there should be only one IA option, as we send 
    // separate RENEW for each IA, but we check all options anyway)
    while ( opt = Reply->getOption() ) 
    {
        switch (opt->getOptType()) 
        {
            case OPTION_IA:
            {
                SmartPtr<TClntOptIA_NA> ptrOptIA = (Ptr*)opt;
                ptrOptIA->setThats(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
				   ptrDUID->getDUID(), SmartPtr<TIPv6Addr>() /*NULL*/, Reply->getIface());
                //FIXME: replace this NULL with something meaningfull
                //       (required for Unicast to work correctly)
                ptrOptIA->doDuties();
                if (ptrOptIA->getStatusCode()==STATUSCODE_SUCCESS)
                {
                    //if we have received enough addresses,
                    //remove assigned IA's by server from request message
                    SmartPtr<TOpt> requestOpt;
                    Options.first();
                    while (requestOpt = Options.get())
                    {
                        if (requestOpt->getOptType()==OPTION_IA)
                        {
                            SmartPtr<TClntOptIA_NA> ptrIA = (Ptr*) requestOpt;
                            if ((ptrIA->getIAID() == ptrOptIA->getIAID() ) &&
                                (ClntCfgMgr->countAddrForIA(ptrIA->getIAID()) == ptrIA->countAddr()) )
                            {
                                //found this IA, it has enough addresses and everything is ok.
                                //Shortly, we have this IA configured.
                                Options.del();
                                break;
                            }
                        } //if
                    } //while
                }
                break;
            }
            case OPTION_ORO:
            case OPTION_RELAY_MSG:
            case OPTION_INTERFACE_ID:
            case OPTION_IAADDR:
            case OPTION_RECONF_MSG:
            {
                clog << logger::logWarning << "Illegal option (" << opt->getOptType() 
                    << ") in received REPLY message." << logger::endl;
                break;
            }
            default:
            {             
                // what to do with unknown/other options? execute them
                opt->setParent(this);
                if (opt->doDuties()) 
                {
                    SmartPtr<TOpt> requestOpt;
                    Options.first();
                    while ( requestOpt = Options.get()) {
                        if (requestOpt->getOptType()==opt->getOptType()) 
                        {
                            if (ptrOptionReqOpt&&(ptrOptionReqOpt->isOption(opt->getOptType())))
                                ptrOptionReqOpt->delOption(opt->getOptType());
                            Options.del();
                        }//if
                    }//while
                }
            }
        }
    }
    //Here we received answer from our server, which updated the "whole information"
    //There is no use to send Rebind even if server realesed some addresses/IAs
    //in such a case new Solicit message should be sent
    //Also if some configuration parameters weren't sustain they should be changed
    IsDone = true;
}

void TClntMsgRenew::releaseIA(long IAID)
{
    // setStatus(NOTCONFIGURED);
    //FIXME:
}

void TClntMsgRenew::doDuties()
{
    // FIXME: increase RT from REN_TIMEOUT to REN_MAX_RT

    // should we send RENEW once more or start sending REBIND
    if (!MRD) 
    {
        std::clog << logger::logNotice 
            << "RENEW remains unanswered and timeout T2 reached. " << logger::endl;
        ClntTransMgr->sendRebind(this->Options,this->getIface());
        IsDone = true;
        return;
    }
    //In other case
    //FIXME:IF
    //      in any of renewed IAs all addresses expired (before reaching T2 timeout)
    //      in such Server didn't want IAs to be rebind in other server
    //ELSE
    send();
}


bool TClntMsgRenew::check()
{
    // this should never happen
    return false;
}

TClntMsgRenew::~TClntMsgRenew()
{
    delete pkt;
    pkt = NULL;
}
