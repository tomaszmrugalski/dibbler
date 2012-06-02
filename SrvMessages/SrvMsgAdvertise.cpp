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
#include "OptOptionRequest.h"
#include "OptDUID.h"
#include "SrvOptIA_NA.h"
#include "SrvOptTA.h"
#include "OptStatusCode.h"
#include "SrvOptFQDN.h"
#include "SrvOptIA_PD.h"
#include "SrvTransMgr.h"
#include "Logger.h"

using namespace std;

TSrvMsgAdvertise::TSrvMsgAdvertise(SPtr<TSrvMsgSolicit> solicit)
    :TSrvMsg(solicit->getIface(),solicit->getAddr(), ADVERTISE_MSG, 
	     solicit->getTransID())
{
    getORO( (Ptr*)solicit );
    copyClientID( (Ptr*)solicit );
    copyRelayInfo( (Ptr*)solicit );
    copyAAASPI( (Ptr*)solicit );
    copyRemoteID( (Ptr*)solicit );

    if (!handleSolicitOptions(solicit)) {
	IsDone = true;
	return;
    }
    IsDone = false;
}

bool TSrvMsgAdvertise::handleSolicitOptions(SPtr<TSrvMsgSolicit> solicit) {

    processOptions((Ptr*)solicit, true); // quietly

    // append serverID, preference and possibly unicast
    appendMandatoryOptions(ORO);
    
    //if client requested parameters and policy doesn't forbid from answering
    appendRequestedOptions(ClientDUID, PeerAddr, Iface, ORO);

    appendStatusCode();

    // this is ADVERTISE only, so we need to release assigned addresses
    releaseAll(true); // release it quietly

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

std::string TSrvMsgAdvertise::getName() const{
    return "ADVERTISE";
}
