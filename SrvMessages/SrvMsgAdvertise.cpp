/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgAdvertise.cpp,v 1.9 2004-06-20 21:00:45 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.7  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 * Revision 1.6  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 *                                                                           
 */

#include "SrvMsgAdvertise.h"
#include "SrvMsg.h"
#include "Logger.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptIA_NA.h"
#include "OptStatusCode.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptPreference.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "SrvOptTimeZone.h"
#include "SrvOptDomainName.h"
#include "AddrClient.h"

#include "Logger.h"

TSrvMsgAdvertise::TSrvMsgAdvertise(SmartPtr<TSrvIfaceMgr> IfaceMgr,
				   SmartPtr<TSrvTransMgr> TransMgr,
				   SmartPtr<TSrvCfgMgr> CfgMgr,
				   SmartPtr<TSrvAddrMgr> AddrMgr,
				   SmartPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,
	     solicit->getIface(),solicit->getAddr(), ADVERTISE_MSG, 
	     solicit->getTransID())
{
    SmartPtr<TOpt>       opt;
    SmartPtr<TSrvOptClientIdentifier> optClntID;
    SmartPtr<TDUID>      clntDuid;
    SmartPtr<TIPv6Addr>  clntAddr;
    unsigned int         clntIface;

    opt = solicit->getOption(OPTION_CLIENTID);
    optClntID = (Ptr*) opt;
    clntDuid = optClntID->getDUID();
    clntAddr = solicit->getAddr();
    clntIface =solicit->getIface();

    // is this client supported?
    if (!CfgMgr->isClntSupported(clntDuid, clntAddr, clntIface)) {
        //No reply for this client 
	Log(Notice) << "Client with DUID=" << clntDuid << "/addr=" << clntAddr 
		    << " was rejected (due to accept-only or reject-client)." << LogEnd;
        IsDone=true;
        return;
    }

    SmartPtr<TSrvOptOptionRequest> reqOpts;

    //remember requested option in order to add number of "hint" options,
    //wich are included in this packet (but not in OPTION REQUEST option).
    //if OPTION REQUEST option wasn't included by client - create new one
    reqOpts= (Ptr*) solicit->getOption(OPTION_ORO);
    if (!reqOpts)
        reqOpts=new TSrvOptOptionRequest(this);
    
    // --- process this message ---
    solicit->firstOption();
    while ( opt = solicit->getOption()) {
        switch (opt->getOptType()) {
	case OPTION_CLIENTID : {
	    this->Options.append(opt);
	    break;
	}
	case OPTION_IA : {
	    SmartPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA = new TSrvOptIA_NA(AddrMgr, CfgMgr, (Ptr*) opt,
					clntDuid, clntAddr, 
					clntIface, SOLICIT_MSG,this);
	    this->Options.append((Ptr*)optIA_NA);
	    break;
	}
	case OPTION_RAPID_COMMIT: {
	    // RAPID COMMIT present, but we're in ADVERTISE, so obviously
	    // server is configured not to use RAPID COMMIT
	    Log(Notice) << "Generating ADVERTISE message, RAPID COMMIT option ignored." << LogEnd;
	    break;
	}
	case OPTION_IAADDR: {
	    Log(Warning) << "Invalid(misplaced) IAADDR option received." << LogEnd;
	    break;
	}
	case OPTION_ORO: 
	case OPTION_ELAPSED_TIME : {

	    break;
	}
	case OPTION_STATUS_CODE : {
	    SmartPtr< TOptStatusCode > ptrStatus = (Ptr*) opt;
	    Log(Error) << "Receviced STATUS_CODE from client:" 
		       <<  ptrStatus->getCode() << ", (" << ptrStatus->getText()
		       << ")" << logger::endl;
	    break;
	}
	    
        //add options requested by client to option Request Option if
       //client didn't included them

	case OPTION_DNS_RESOLVERS: {
	    if (!reqOpts->isOption(OPTION_DNS_RESOLVERS))
		reqOpts->addOption(OPTION_DNS_RESOLVERS);
	    break;
	}
	case OPTION_DOMAIN_LIST: {
	    if (!reqOpts->isOption(OPTION_DOMAIN_LIST))
		reqOpts->addOption(OPTION_DOMAIN_LIST);
	    break;
	}
	case OPTION_NTP_SERVERS:
	    if (!reqOpts->isOption(OPTION_NTP_SERVERS))
		reqOpts->addOption(OPTION_NTP_SERVERS);
	    break;
	case OPTION_TIME_ZONE:
	    if (!reqOpts->isOption(OPTION_TIME_ZONE))
		reqOpts->addOption(OPTION_TIME_ZONE);
	    break;
	    
	case OPTION_PREFERENCE :
	case OPTION_UNICAST :
	case OPTION_SERVERID : {
	    std::clog << logger::logWarning 
		      << "Invalid option (OPTION_UNICAST) received." << logger::endl;
	    break;
	}
                // options not yet supported 
	case OPTION_IA_TA    :
	case OPTION_RELAY_MSG :
	case OPTION_AUTH_MSG :
	case OPTION_USER_CLASS :
	case OPTION_VENDOR_CLASS :
	case OPTION_VENDOR_OPTS :
	case OPTION_INTERFACE_ID :
	case OPTION_RECONF_MSG :
	case OPTION_RECONF_ACCEPT:
	default: {
	    std::clog << logger::logDebug << "Option not supported, opttype=" 
		      << opt->getOptType() << logger::endl;
	    break;
	}
	} // end of switch
    } // end of while
    
    //if client requested parameters and policy doesn't forbid from answering
    appendRequestedOptions(clntDuid, clntAddr, clntIface, reqOpts);

    // include our DUID
    SmartPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(CfgMgr->getDUID(),this);
    Options.append((Ptr*)ptrSrvID);

    // ... and our preference
    SmartPtr<TSrvOptPreference> ptrPreference;
    unsigned char preference = CfgMgr->getIfaceByID(solicit->getIface())->getPreference();
    Log(Debug) << "Preference set to " << (int)preference << "." << LogEnd;
    ptrPreference = new TSrvOptPreference(preference,this);
    Options.append((Ptr*)ptrPreference);

    // this is ADVERTISE only, so we need to release assigned addresses
    this->firstOption();
    while ( opt = this->getOption()) {
	if ( opt->getOptType()==OPTION_IA) {
	    SmartPtr<TSrvOptIA_NA> ptrOptIA_NA;
	    ptrOptIA_NA = (Ptr*) opt;
	    // FIXME: quiet should be true
	    ptrOptIA_NA->releaseAllAddrs(false);
	}
    }


    pkt = new char[this->getSize()];
    IsDone = false;
    this->MRT = 0;
    this->send();
}

bool TSrvMsgAdvertise::check()
{
    // this should never happen
    return true;
}

void TSrvMsgAdvertise::answer(SmartPtr<TMsg> Rep)
{
    // this should never happen
    return;
}

TSrvMsgAdvertise::~TSrvMsgAdvertise()
{
}

unsigned long TSrvMsgAdvertise::getTimeout()
{
    return 0;
}
void TSrvMsgAdvertise::doDuties()
{
    IsDone = true;
}

string TSrvMsgAdvertise::getName() {
    return "ADVERTISE";
}
