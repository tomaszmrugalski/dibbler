/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvMsgAdvertise.cpp,v 1.33 2008-11-13 22:40:26 thomson Exp $
 */

#include "SrvMsgAdvertise.h"
#include "Logger.h"
#include "SrvOptOptionRequest.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptIA_NA.h"
#include "SrvOptTA.h"
#include "SrvOptServerUnicast.h"
#include "SrvOptStatusCode.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptPreference.h"
#include "SrvOptDNSServers.h"
#include "SrvOptNTPServers.h"
#include "SrvOptTimeZone.h"
#include "SrvOptDomainName.h"
#include "SrvOptFQDN.h"
#include "SrvOptIA_PD.h"
#include "Logger.h"

TSrvMsgAdvertise::TSrvMsgAdvertise(SPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(solicit->getIface(),solicit->getAddr(), ADVERTISE_MSG, 
	     solicit->getTransID())
{
    getORO( (Ptr*)solicit );
    copyClientID( (Ptr*)solicit );
    copyRelayInfo( (Ptr*)solicit );
    copyAAASPI( (Ptr*)solicit );
    copyRemoteID( (Ptr*)solicit );

    if (!this->handleSolicitOptions(solicit)) {
	this->IsDone = true;
	return;
    }
    this->IsDone = false;
}

bool TSrvMsgAdvertise::handleSolicitOptions(SPtr<TSrvMsgSolicit> solicit) {

    SPtr<TOpt>       opt;
    SPtr<TIPv6Addr> clntAddr = PeerAddr;

    // is this client supported?
    // @todo move this to a common place (for every message)
    if (!SrvCfgMgr().isClntSupported(ClientDUID, clntAddr, Iface)) {
        //No reply for this client 
        Log(Notice) << "Client (DUID=" << ClientDUID->getPlain() << ",addr=" << *clntAddr 
		    << ") was rejected due to accept-only or reject-client." << LogEnd;
        return false;
    }

    // --- process this message ---
    solicit->firstOption();
    while ( opt = solicit->getOption()) {
	switch (opt->getOptType()) {
	case OPTION_IA_NA : {
	    SPtr<TSrvOptIA_NA> optIA_NA;
	    optIA_NA = new TSrvOptIA_NA( (Ptr*)opt, ClientDUID, clntAddr, Iface, SOLICIT_MSG, this);
	    Options.push_back((Ptr*)optIA_NA);
	    break;
	}
	case OPTION_IA_TA: {
	    SPtr<TSrvOptTA> optTA;
	    optTA = new TSrvOptTA((Ptr*) opt, 
				  ClientDUID, clntAddr, Iface, SOLICIT_MSG, this);
	    Options.push_back( (Ptr*) optTA);
	    break;
	}
	case OPTION_IA_PD: {
	    SPtr<TSrvOptIA_PD> optPD;
	    optPD = new TSrvOptIA_PD((Ptr*) opt, clntAddr, ClientDUID,  Iface, SOLICIT_MSG, this);
	    Options.push_back( (Ptr*) optPD);
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
	case OPTION_IAPREFIX: {
	    Log(Warning) << "Invalid(misplaced) IAPREFIX option received." << LogEnd;
	    break;
	}
	case OPTION_AUTH : {
	    ORO->addOption(OPTION_AUTH);
	    break;
	}                 
	case OPTION_ORO:
    case OPTION_CLIENTID:
	case OPTION_ELAPSED_TIME : {
	    break;
	}
	case OPTION_STATUS_CODE : {
	    SPtr< TOptStatusCode > ptrStatus = (Ptr*) opt;
	    Log(Error) << "Received STATUS_CODE from client:" 
		       <<  ptrStatus->getCode() << ", (" << ptrStatus->getText()
		       << ")" << LogEnd;
	    break;
	}
	    
	//add options requested by client to option Request Option if
	//client didn't included them
	    	case OPTION_FQDN : {
	    SPtr<TSrvOptFQDN> requestFQDN = (Ptr*) opt;
	    SPtr<TOptFQDN> anotherFQDN = (Ptr*) opt;
	    string hint = anotherFQDN->getFQDN();
	    SPtr<TSrvOptFQDN> optFQDN;

	    SPtr<TIPv6Addr> clntAssignedAddr = SrvAddrMgr().getFirstAddr(ClientDUID);
	    if (clntAssignedAddr)
		optFQDN = this->prepareFQDN(requestFQDN, ClientDUID, clntAssignedAddr, hint, false);
	    else
		optFQDN = this->prepareFQDN(requestFQDN, ClientDUID, clntAddr, hint, false);

	    if (optFQDN) {
		this->Options.push_back((Ptr*) optFQDN);
	    }
	    break;
	}
	case OPTION_VENDOR_OPTS:
	{
	    SPtr<TOptVendorData> v = (Ptr*) opt;
	    appendVendorSpec(ClientDUID, Iface, v->getVendor(), ORO);
	    break;
	}
	
	case OPTION_AAAAUTH:
	{
	    Log(Debug) << "Auth: Option AAAAuthentication received." << LogEnd;
	    break;
	}
	case OPTION_PREFERENCE:
	case OPTION_UNICAST:
	case OPTION_SERVERID: 
	case OPTION_RELAY_MSG:
	case OPTION_INTERFACE_ID:
	{
	    Log(Warning) << "Invalid option (" << opt->getOptType() << ") received." << LogEnd;
	    break;
	}

	case OPTION_DNS_SERVERS: 
	case OPTION_DOMAIN_LIST: 
	case OPTION_SNTP_SERVERS:
	case OPTION_NEW_TZDB_TIMEZONE: 
	{
	    handleDefaultOption(opt);
	    break;
	}
	// options not yet supported 
	case OPTION_USER_CLASS :
	case OPTION_VENDOR_CLASS:
	case OPTION_RECONF_MSG :
	case OPTION_RECONF_ACCEPT:
	default: {
	    Log(Debug) << "Option " << opt->getOptType() << " is not supported." << LogEnd;
	    break;
	}
	} // end of switch
    } // end of while

    // append serverID, preference and possibly unicast
    appendMandatoryOptions(ORO);
    
    //if client requested parameters and policy doesn't forbid from answering
    appendRequestedOptions(ClientDUID, clntAddr, Iface, ORO);


    appendStatusCode();

    // this is ADVERTISE only, so we need to release assigned addresses
    this->firstOption();
    while ( opt = this->getOption()) {
	switch (opt->getOptType()) {
	case OPTION_IA_NA: {
	    SPtr<TSrvOptIA_NA> ptrOptIA_NA;
	    ptrOptIA_NA = (Ptr*) opt;
	    ptrOptIA_NA->releaseAllAddrs(false);
	    break;
	}
	case OPTION_IA_TA: {
	    SPtr<TSrvOptTA> ta;
	    ta = (Ptr*) opt;
	    ta->releaseAllAddrs(false);
	    break;
	}
	default:
	    break;
	}
    }

    appendAuthenticationOption(ClientDUID);

    pkt = new char[this->getSize()];
    MRT = 0;
    send();
    return true;
}

bool TSrvMsgAdvertise::check() {
    // this should never happen
    return true;
}

TSrvMsgAdvertise::~TSrvMsgAdvertise() {
}

unsigned long TSrvMsgAdvertise::getTimeout() {
    return 0;
}
void TSrvMsgAdvertise::doDuties() {
    IsDone = true;
}

string TSrvMsgAdvertise::getName() {
    return "ADVERTISE";
}
