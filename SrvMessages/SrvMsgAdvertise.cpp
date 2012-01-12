/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
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

    // --- process this message ---
    solicit->firstOption();
    while ( opt = solicit->getOption()) {
	switch (opt->getOptType()) {
	case OPTION_IA_NA : {
            processIA_NA((Ptr*)solicit, (Ptr*) opt);
	    break;
	}
	case OPTION_IA_TA: {
            processIA_TA((Ptr*)solicit, (Ptr*) opt);
            break;
	}
	case OPTION_IA_PD: {
            processIA_PD((Ptr*)solicit, (Ptr*) opt);
	    break;
	}
	case OPTION_AUTH : {
	    ORO->addOption(OPTION_AUTH);
	    break;
	}                 
        case OPTION_FQDN : {
            // skip processing for now (we need to process all IA_NA/IA_TA first)
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
	case OPTION_STATUS_CODE : {
	    Log(Warning) << "Invalid option (" << opt->getOptType() << ") received. Client is not supposed to send it. Option ignored." << LogEnd;
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

    opt = solicit->getOption(OPTION_FQDN);
    if (opt) {
        processFQDN((Ptr*)solicit, (Ptr*) opt);
    }

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
        case OPTION_IA_PD: {
            SPtr<TSrvOptIA_PD> pd;
            pd = (Ptr*) opt;
            pd->releaseAllPrefixes(false);
        }
	default: {
	    continue;
	}
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
