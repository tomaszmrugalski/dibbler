/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence                                
 *                                                                           
 * $Id: SrvMsgAdvertise.cpp,v 1.14.2.1 2006-02-05 23:38:08 thomson Exp $
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
    this->copyRelayInfo((Ptr*)solicit);
    if (!this->answer(solicit)) {
	this->IsDone = true;
	return;
    }
    this->IsDone = false;
}

bool TSrvMsgAdvertise::answer(SmartPtr<TSrvMsgSolicit> solicit) {
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
    if (!SrvCfgMgr->isClntSupported(clntDuid, clntAddr, clntIface)) {
        //No reply for this client 
	Log(Notice) << "Client (DUID=" << clntDuid->getPlain() << ",addr=" << *clntAddr 
		    << ") was rejected due to accept-only or reject-client." << LogEnd;
        return false;
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
	    optIA_NA = new TSrvOptIA_NA(SrvAddrMgr, SrvCfgMgr, (Ptr*) opt,
					clntDuid, clntAddr, 
					clntIface, SOLICIT_MSG,this);
	    this->Options.append((Ptr*)optIA_NA);
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TSrvOptTA> optTA;
	    optTA = new TSrvOptTA(SrvAddrMgr, SrvCfgMgr, (Ptr*) opt, 
				  clntDuid, clntAddr, clntIface, SOLICIT_MSG, this);
	    this->Options.append( (Ptr*) optTA);
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
		       << ")" << LogEnd;
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
	    Log(Warning) << "Invalid option (OPTION_UNICAST) received." << LogEnd;
	    break;
	}
	    // options not yet supported 
	case OPTION_RELAY_MSG :
	case OPTION_AUTH_MSG :
	case OPTION_USER_CLASS :
	case OPTION_VENDOR_CLASS :
	case OPTION_VENDOR_OPTS :
	case OPTION_INTERFACE_ID :
	case OPTION_RECONF_MSG :
	case OPTION_RECONF_ACCEPT:
	default: {
	    Log(Debug) << "Option " << opt->getOptType() << "is not supported." << LogEnd;
	    break;
	}
	} // end of switch
    } // end of while
    
    //if client requested parameters and policy doesn't forbid from answering
    this->appendRequestedOptions(clntDuid, clntAddr, clntIface, reqOpts);

    // include our DUID
    SmartPtr<TSrvOptServerIdentifier> ptrSrvID;
    ptrSrvID = new TSrvOptServerIdentifier(SrvCfgMgr->getDUID(),this);
    Options.append((Ptr*)ptrSrvID);

    // ... and our preference
    SmartPtr<TSrvOptPreference> ptrPreference;
    unsigned char preference = SrvCfgMgr->getIfaceByID(solicit->getIface())->getPreference();
    Log(Debug) << "Preference set to " << (int)preference << "." << LogEnd;
    ptrPreference = new TSrvOptPreference(preference,this);
    Options.append((Ptr*)ptrPreference);

    // does this server support unicast?
    SmartPtr<TIPv6Addr> unicastAddr = SrvCfgMgr->getIfaceByID(solicit->getIface())->getUnicast();
    if (unicastAddr) {
	SmartPtr<TSrvOptServerUnicast> optUnicast = new TSrvOptServerUnicast(unicastAddr, this);
	Options.append((Ptr*)optUnicast);
    }

    // this is ADVERTISE only, so we need to release assigned addresses
    this->firstOption();
    while ( opt = this->getOption()) {
	switch (opt->getOptType()) {
	case OPTION_IA: {
	    SmartPtr<TSrvOptIA_NA> ptrOptIA_NA;
	    ptrOptIA_NA = (Ptr*) opt;
	    ptrOptIA_NA->releaseAllAddrs(false);
	    break;
	}
	case OPTION_IA_TA: {
	    SmartPtr<TSrvOptTA> ta;
	    ta = (Ptr*) opt;
	    ta->releaseAllAddrs(false);
	    break;
	}
	default:
	    break;
	}
    }

    pkt = new char[this->getSize()];
    this->MRT = 0;
    this->send();
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

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2005/03/15 23:02:31  thomson
 * 0.4.0 release.
 *
 * Revision 1.13  2005/03/15 00:36:22  thomson
 * 0.4.0 release (win32 commit)
 *
 * Revision 1.12  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.11  2004/12/02 00:51:06  thomson
 * Log files are now always created (bugs #34, #36)
 *
 * Revision 1.10  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.9  2004/06/20 21:00:45  thomson
 * Various fixes.
 *
 * Revision 1.8  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.7  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 * Revision 1.6  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 */
