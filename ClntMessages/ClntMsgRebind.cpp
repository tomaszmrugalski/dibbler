#include "SmartPtr.h"
#include "ClntMsg.h"
#include "ClntMsgRebind.h"
#include "ClntOptIA_NA.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptServerUnicast.h"
#include "AddrIA.h"
#include "Logger.h"
#include "ClntOptOptionRequest.h"
#include <cmath>

/*TClntMsgRebind::TClntMsgRebind(SmartPtr<TClntIfaceMgr> IfaceMgr, 
			       SmartPtr<TClntTransMgr> TransMgr, 
			       SmartPtr<TClntCfgMgr> CfgMgr, 
			       SmartPtr<TClntAddrMgr> AddrMgr,
			       SmartPtr<TAddrIA> ptrIA)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,ptrIA->getIface(), NULL ,REBIND_MSG)
{
    // remember this AddrIA;
    this->AddrIA = ptrIA;

    SmartPtr<TClntOptIA_NA> ptrOptIA = new TClntOptIA_NA(ptrIA,this);
    Options.append( (Ptr*) ptrOptIA);
    
    SmartPtr<TClntOptClientIdentifier> ptrClntID = 
	new TClntOptClientIdentifier( CfgMgr->getDUID(),this);
    Options.append( (Ptr*) ptrClntID);

    IRT=REB_TIMEOUT;
    MRT=REB_MAX_RT;

    MRC=0;

    RT = ptrIA->getValidTimeout();

    // retransmit until any addr expires
    MRD= ptrIA->getMaxValid();
    this->IsDone = false;
}*/
TClntMsgRebind::TClntMsgRebind(SmartPtr<TClntIfaceMgr> IfaceMgr, 
			       SmartPtr<TClntTransMgr> TransMgr, 
			       SmartPtr<TClntCfgMgr> CfgMgr, 
			       SmartPtr<TClntAddrMgr> AddrMgr,
			       TContainer<SmartPtr<TOpt> > ptrOpts, int iface)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface, SmartPtr<TIPv6Addr>() /*NULL*/, REBIND_MSG)
{
    Options=ptrOpts;
    IRT=REB_TIMEOUT;
    MRT=REB_MAX_RT;
    MRC=0;
    RT=0;

    SmartPtr<TOpt> ptrOpt;
    firstOption();
    while(ptrOpt=getOption())
    {
        if (ptrOpt->getOptType()==OPTION_ELAPSED_TIME)
            Options.del();
        if (ptrOpt->getOptType()==OPTION_SERVERID)
            Options.del();
    };
    
    unsigned long maxMRD=0;    
    firstOption();
    while(ptrOpt=getOption())
    {
        if (ptrOpt->getOptType()==OPTION_IA)
        {
            SmartPtr<TClntOptIA_NA> ptrIA=(Ptr*) ptrOpt;
            SmartPtr<TAddrIA> ptrAddrIA=
                this->ClntAddrMgr->getIA(ptrIA->getIAID());

            if (maxMRD<ptrAddrIA->getMaxValidTimeout())
                maxMRD=ptrAddrIA->getMaxValidTimeout();
        }
    }
    MRD= maxMRD;
    pkt = new char[getSize()];
    this->IsDone = false;    
}

void TClntMsgRebind::answer(SmartPtr<TMsg> Reply)
{
        SmartPtr<TOpt> opt;
    
    // get DUID
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) Reply->getOption(OPTION_SERVERID);
    
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
                    ptrDUID->getDUID(), SmartPtr<TIPv6Addr>() /*NULL*/,Reply->getIface());
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

    /*SmartPtr<TOpt> opt;
    SmartPtr<TClntOptServerIdentifier> optSrvDUID = (Ptr*) Reply->getOption(OPTION_SERVERID);
    SmartPtr<TClntOptServerUnicast> optUnicast = (Ptr*) Reply->getOption(OPTION_UNICAST);
    
    Reply->firstOption();
    
    // for each option in message... (there should be only one IA option, as we send 
    // separate REBIND for each IA, but we check all options anyway)
    while ( opt = Reply->getOption() ) {
	switch (opt->getOptType()) {
	case OPTION_IA: {
	    SmartPtr< TClntOptIA_NA> ptrOptIA = (Ptr*)opt;
	    this->updateIA(ptrOptIA, optSrvDUID, optUnicast);
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
	    // what to do with unknown/other options? execute them
	    opt->doDuties();
	}
    }
    IsDone = true;*/
}

void TClntMsgRebind::updateIA(SmartPtr <TClntOptIA_NA> ptrOptIA,
			      SmartPtr<TClntOptServerIdentifier> optSrvDUID, 
			      SmartPtr<TClntOptServerUnicast> optUnicast) {

    SmartPtr< TAddrIA> ptrAddrIA;
    bool found = false;
    
    // ..find IA in addrMgr...
    ClntAddrMgr->firstIA();
    while  (ptrAddrIA = ClntAddrMgr->getIA() ) {
	if (ptrOptIA->getIAID() == ptrAddrIA->getIAID()) {
	    found = true;
	    break;
	}
    }

    if (found) {

	// set new server's DUID to handle this IA
	ptrAddrIA->setDUID(optSrvDUID->getDUID());

	//
	if (optUnicast) {
	    ptrAddrIA->setUnicast(optUnicast->getAddr());
	} else {
	    ptrAddrIA->setMulticast();
	}

	// IAID found, set up new received options.
	SmartPtr<TAddrAddr> ptrAddrAddr;
	SmartPtr<TClntOptIAAddress> ptrOptAddr;
	
	// are all addrs configured?
	if (ptrOptIA->countAddr() != ptrAddrIA->countAddr() ) {
	    this->releaseIA(ptrOptIA->getIAID());
	}
	
	// for each address in IA option...
	ptrOptIA->firstAddr();
	while (ptrOptAddr = ptrOptIA->getAddr() ) {
	    ptrAddrAddr = ptrAddrIA->getAddr( ptrOptAddr->getAddr() );
	    if (!ptrAddrAddr) {
		// there is no such addr in db
		// FIXME: what to do with new addrs?
		// (Thomson:I think we should RELEASE all addrs, and issue new REQUEST for this IA)
	    } else {
		if ( (ptrOptAddr->getPref() == 0) || (ptrOptAddr->getValid() == 0) ) {
		    releaseIA( ptrOptIA->getIAID() );
		    break; // analyze next option OPTION_IA
		}
		// set up new options
		if (ptrOptAddr->getPref() != ptrAddrAddr->getPref() )
		{
		    // received diffrent prefered-lifetime
		    // FIXME:
		}
		if (ptrOptAddr->getValid() != ptrAddrAddr->getValid() )
		{
		    // received diffrent prefered-lifetime
		    // FIXME:
		}
		ptrAddrAddr->setTimestamp();
	    }
	}
	ptrAddrIA->setT1( ptrOptIA->getT1() );
	ptrAddrIA->setT2( ptrOptIA->getT2() );
	ptrAddrIA->setTimestamp();
	ptrAddrIA->setState(CONFIGURED);
    } else {
	// unknown IAID, ignore it
	clog << logger::logWarning << "Received message contains unknown IA (IAID="
	     << ptrOptIA->getIAID() << "). Ignoring it." << logger::endl;
    }
}

void TClntMsgRebind::doDuties()
{
    SmartPtr<TClntOptIA_NA> ptrIA=(Ptr*)getOption(OPTION_IA);
    SmartPtr<TAddrIA> ptrAddrIA = ClntAddrMgr->getIA(ptrIA->getIAID());

    if (!MRD)
    {
        SmartPtr<TOpt> ptrOpt;
        firstOption();
        while(ptrOpt=getOption())
        {
            if (ptrOpt->getOptType()!=OPTION_IA)
                continue;
            SmartPtr<TClntOptIA_NA> ptrIA=(Ptr*)ptrOpt;
            releaseIA(ptrIA->getIAID());
        }
        IsDone=true;
    }
    else
      this->send();
}

void TClntMsgRebind::releaseIA(int IAID)
{
    SmartPtr<TAddrIA> ptrAddrIA=this->ClntAddrMgr->getIA(IAID);
    if (!ptrAddrIA)
    {
        std::clog<<logger::logError<<"IA has not been found in Address Manager."<< logger::endl;
        return;
    }
    //FIXME: release all addrs
    SmartPtr<TAddrAddr> ptrAddr;
    ptrAddrIA->firstAddr();
    while(ptrAddr=ptrAddrIA->getAddr())
    {
        //remove outdated address from interface
        ClntIfaceMgr->getIfaceByID(ptrAddrIA->getIface())->delAddr(ptrAddr->get());
        //and from db
        ptrAddrIA->delAddr(ptrAddr->get());
    }
    ptrAddrIA->setState(NOTCONFIGURED);
}

bool TClntMsgRebind::check()
{
	return 0;
}

TClntMsgRebind::~TClntMsgRebind()
{
}
