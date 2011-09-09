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

#include "SrvMsgReconfigure.h"
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

TSrvMsgReconfigure::TSrvMsgReconfigure(int iface, SPtr<TIPv6Addr> clientAddr, SPtr<TIPv6Addr> ia ,int msgType, SPtr<TDUID> ptrDUID)
    :TSrvMsg(iface, clientAddr, RECONFIGURE_MSG, rand()%(2^24) )
{
    //getORO( (Ptr*)solicit );
    //copyClientID( (Ptr*)solicit );
    //copyRelayInfo( clientAddr );
    //copyAAASPI( (Ptr*)solicit );
    //copyRemoteID( (Ptr*)solicit );
ClientDUID=ptrDUID;

    // append serverID, preference and possibly unicast
    //appendMandatoryOptions(ORO);
    
    //if client requested parameters and policy doesn't forbid from answering
    //appendRequestedOptions(ClientDUID, clientAddr, Iface, ORO);


    appendStatusCode();

//SPtr<TIPv6Addr> clntAddr = PeerAddr;

    // this is ADVERTISE only, so we need to release assigned addresses
    if(msgType==1) {
Log(Crit) << " wwwwwwwwwww " << PeerAddr->getPlain() << LogEnd;

	    //ptrOptIA_NA->releaseAllAddrs(true);
//Options.push_back(new TOptInteger(OPTION_ADDRPARAMS, 2, 0, this));
Log(Crit) << " jestem_na_warunku_msgType=1 " << LogEnd;
	    //break;
	}

	else {
	    SPtr<TSrvOptTA> ta;
	    //ta = (Ptr*) opt;
	    //ta->releaseAllAddrs(false);
	    //break;
	}
	//default:
	//    break;
	

    appendAuthenticationOption(ClientDUID);

   // pkt = new char[this->getSize()];
   // MRT = 0;
Log(Crit) << "przed wysylka " << LogEnd;
    //send();
    return;
}

bool TSrvMsgReconfigure::check() {
    // this should never happen
    return true;
}

TSrvMsgReconfigure::~TSrvMsgReconfigure() {
}

unsigned long TSrvMsgReconfigure::getTimeout() {
    return 0;
}
void TSrvMsgReconfigure::doDuties() {
    IsDone = true;
}

string TSrvMsgReconfigure::getName() {
    return "RECONFIGURE";
}
