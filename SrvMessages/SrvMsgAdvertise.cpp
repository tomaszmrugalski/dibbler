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
#include "OptDUID.h"
#include "OptOptionRequest.h"
#include "OptStatusCode.h"
#include "SrvOptFQDN.h"
#include "SrvOptIA_NA.h"
#include "SrvOptIA_PD.h"
#include "SrvOptTA.h"
#include "SrvTransMgr.h"

using namespace std;

TSrvMsgAdvertise::TSrvMsgAdvertise(SPtr<TSrvMsg> solicit)
    : TSrvMsg(solicit->getIface(), solicit->getRemoteAddr(), ADVERTISE_MSG, solicit->getTransID()) {
  getORO(SPtr_cast<TMsg>(solicit));
  copyClientID(SPtr_cast<TMsg>(solicit));
  copyRelayInfo(solicit);
  copyAAASPI(solicit);
  copyRemoteID(solicit);

  if (!handleSolicitOptions(solicit)) {
    IsDone = true;
    return;
  }
  IsDone = false;
}

bool TSrvMsgAdvertise::handleSolicitOptions(SPtr<TSrvMsg> solicit) {

  processOptions(solicit, true);  // quietly

  // append serverID, preference and possibly unicast
  appendMandatoryOptions(ORO);

  // if client requested parameters and policy doesn't forbid from answering
  appendRequestedOptions(ClientDUID, PeerAddr_, Iface, ORO);

  appendStatusCode();

  // this is ADVERTISE only, so we need to release assigned addresses
  releaseAll(true);  // release it quietly

  appendAuthenticationOption(ClientDUID);

  MRT_ = 0;
  return true;
}

bool TSrvMsgAdvertise::check() {
  // this should never happen
  return true;
}

TSrvMsgAdvertise::~TSrvMsgAdvertise() {}

unsigned long TSrvMsgAdvertise::getTimeout() { return 0; }
void TSrvMsgAdvertise::doDuties() { IsDone = true; }

std::string TSrvMsgAdvertise::getName() const { return "ADVERTISE"; }
