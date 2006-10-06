/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRenew.cpp,v 1.7 2006-10-06 00:43:28 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005-01-08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.5  2004/12/08 00:15:49  thomson
 * Issues with denied RENEW (bug #53), code clean up
 *
 * Revision 1.4  2004/12/02 00:51:04  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.3  2004/10/25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

#include "ClntMsgRenew.h"
#include "DHCPConst.h"
#include "ClntOptIA_NA.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptOptionRequest.h"
#include "ClntOptStatusCode.h"
#include "Logger.h"
#include "SmartPtr.h"

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

    pkt = new char[getSize()];
    this->IsDone = false;
    this->send();
}

void TClntMsgRenew::answer(SmartPtr<TClntMsg> Reply)
{
    SmartPtr<TOpt> opt;
    
    // get DUID
    SmartPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = (Ptr*) this->getOption(OPTION_SERVERID);
    
    SmartPtr<TClntOptOptionRequest> ptrOptionReqOpt=(Ptr*)getOption(OPTION_ORO);

    Reply->firstOption();
    // for each option in message... (there should be only one IA option, as we send 
    // separate RENEW for each IA, but we check all options anyway)
    while ( opt = Reply->getOption() ) {
        switch (opt->getOptType()) {
	case OPTION_IA: {
	    SmartPtr<TClntOptIA_NA> ptrOptIA = (Ptr*)opt;
	    if (ptrOptIA->getStatusCode()!=STATUSCODE_SUCCESS) {
		SmartPtr<TClntOptStatusCode> status = (Ptr*) ptrOptIA->getOption(OPTION_STATUS_CODE);
		Log(Warning) << "IA with status code " << 
		    StatusCodeToString(status->getCode()) << ": " 
			     << status->getText() << LogEnd;
		break;
	    }
	    ptrOptIA->setThats(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
			       ptrDUID->getDUID(), SmartPtr<TIPv6Addr>() /*NULL*/, Reply->getIface());

	    ptrOptIA->doDuties();
	    break;
	}
	case OPTION_ORO:
	case OPTION_RELAY_MSG:
	case OPTION_INTERFACE_ID:
	case OPTION_IAADDR:
	case OPTION_RECONF_MSG:
	    Log(Warning) << "Illegal option (" << opt->getOptType() 
			 << ") in received REPLY message." << LogEnd;
	    break;
	default:
	    // what to do with unknown/other options? execute them
	    opt->setParent(this);
	    opt->doDuties();
	}
    }

    //Here we received answer from our server, which updated the "whole information"
    //There is no use to send Rebind even if server realesed some addresses/IAs
    //in such a case new Solicit message should be sent
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
	Log(Notice) << "RENEW remains unanswered and timeout T2 reached. " << LogEnd;
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

string TClntMsgRenew::getName() {
    return "RENEW";
}

TClntMsgRenew::~TClntMsgRenew()
{
    delete pkt;
    pkt = NULL;
}
