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

#include "ClntMsgConfirm.h"
#include "ClntOptIA_NA.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "OptDUID.h"
#include "OptStatusCode.h"
#include "SmartPtr.h"

//  iaLst - contain all IA's to  be checked (they have to be in the same link)
TClntMsgConfirm::TClntMsgConfirm(unsigned int iface, List(TAddrIA) iaLst)
    : TClntMsg(iface, SPtr<TIPv6Addr>(), CONFIRM_MSG) {

  IRT = CNF_TIMEOUT;
  MRT = CNF_MAX_RT;
  MRC = 0;
  MRD = CNF_MAX_RD;

  // The client sets the "msg-type" field to CONFIRM.  The client
  // generates a transaction ID and inserts this value in the
  //"transaction-id" field.

  // The client MUST include a Client Identifier option to identify itself
  // to the server.
  Options.push_back(new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this));
  // The client includes IA options for all of the IAs
  // assigned to the interface for which the Confirm message is being
  // sent.  The IA options include all of the addresses the client
  // currently has associated with those IAs.  The client SHOULD set the
  // T1 and T2 fields in any IA_NA options, and the preferred-lifetime and
  // valid-lifetime fields in the IA Address options to 0, as the server
  // will ignore these fields.
  SPtr<TAddrIA> ia;
  iaLst.first();
  while (ia = iaLst.get()) Options.push_back(new TClntOptIA_NA(ia, true, this));

  appendRequestedOptions();
  appendElapsedOption();
  appendAuthenticationOption();

  IsDone = false;
  send();
}

void TClntMsgConfirm::answer(SPtr<TClntMsg> reply) {
  SPtr<TOptStatusCode> status = reply->getStatusCode();
  if (!status) {
    Log(Warning) << "Received malformed REPLY for CONFIRM: no status option." << LogEnd;
    addrsRejected();
    return;
  }
  switch (status->getCode()) {
    case STATUSCODE_SUCCESS:
      addrsAccepted();
      IsDone = true;
      break;
    case STATUSCODE_NOTONLINK:
      addrsRejected();
      IsDone = true;
      break;
    case STATUSCODE_UNSPECFAIL:
    case STATUSCODE_NOADDRSAVAIL:
    case STATUSCODE_NOBINDING:
    case STATUSCODE_USEMULTICAST:
    default:
      Log(Warning) << "REPLY for CONFIRM received with invalid (" << status->getCode()
                   << ") status code." << LogEnd;
      break;
  }
  return;
}

void TClntMsgConfirm::addrsAccepted() {
  SPtr<TAddrIA> ptrIA;
  SPtr<TOpt> ptrOpt;
  this->firstOption();
  SPtr<TAddrAddr> ptrAddrAddr;
  while (ptrOpt = this->getOption()) {
    if (ptrOpt->getOptType() != OPTION_IA_NA) continue;

    SPtr<TClntOptIA_NA> ptrOptIA = SPtr_cast<TClntOptIA_NA>(ptrOpt);
    if (!ptrOptIA) {
      continue;
    }

    ptrIA = ClntAddrMgr().getIA(ptrOptIA->getIAID());
    if (!ptrIA) {
      continue;
    }

    // Uncomment this line if you don't want RENEW to be sent after
    // CONFIRM exchange is complete
    ptrIA->setState(STATE_CONFIGURED);

    // Once confirmed, this triggers the
    ptrIA->setTimestamp();
    ts = ptrIA->getTimestamp();
    ptrIA->setTimestamp(ts - ptrIA->getT1());

    SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(ptrIA->getIfindex());
    if (!ptrIface) continue;

    ptrIA->firstAddr();
    while (ptrAddrAddr = ptrIA->getAddr()) {
      ptrIface->addAddr(ptrAddrAddr->get(), ptrAddrAddr->getPref(), ptrAddrAddr->getValid(),
                        ptrIface->getPrefixLength());
    }
  }
}

void TClntMsgConfirm::addrsRejected() {
  SPtr<TAddrIA> ptrIA;
  SPtr<TOpt> ptrOpt;
  this->firstOption();
  while (ptrOpt = this->getOption()) {
    if (ptrOpt->getOptType() != OPTION_IA_NA) continue;

    SPtr<TClntOptIA_NA> ptrOptIA = SPtr_cast<TClntOptIA_NA>(ptrOpt);
    if (!ptrOptIA) {
      continue;
    }
    ptrIA = ClntAddrMgr().getIA(ptrOptIA->getIAID());
    if (!ptrIA) continue;

    // release all addrs
    SPtr<TIfaceIface> ptrIface;
    ptrIface = ClntIfaceMgr().getIfaceByID(ptrIA->getIfindex());
    if (!ptrIface) {
      Log(Crit) << "We have addresses assigned to non-existing interface."
                   "Help! Somebody stole an interface!"
                << LogEnd;
      ptrIA->setState(STATE_NOTCONFIGURED);
      return;
    }
    SPtr<TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while (ptrAddr = ptrIA->getAddr()) {
      // remove addr from ...
      ptrIface->delAddr(ptrAddr->get(), ptrIface->getPrefixLength());
      // ... and from DB
      ptrIA->delAddr(ptrAddr->get());
    }

    // not only the address should be rejected, but also the original IA should
    // be deleted and using a new IA to process SOLICIT
    SPtr<TClntCfgIA> ptrCfgIA = ClntCfgMgr().getIA(ptrOptIA->getIAID());
    ptrCfgIA->reset();
    ptrIA->reset();
  }
  ClntAddrMgr().firstIA();
}

void TClntMsgConfirm::doDuties() {
  // The first Confirm message from the client on the interface MUST be
  // delayed by a random amount of time between 0 and CNF_MAX_DELAY.
  /// @todo: MRD counters are a mess.
  if (!MRD) {
    // MRD reached. Nobody said that out addrs are faulty, so we suppose
    // they are ok. Use them
    Log(Info) << "MRD reached and there is no valid response for CONFIRM. "
              << "Assuming addresses are valid." << LogEnd;
    addrsAccepted();
    IsDone = true;
    return;
  }

  send();
}

unsigned long TClntMsgConfirm::getTimeout() { return 0; }

bool TClntMsgConfirm::check() { return 0; }

std::string TClntMsgConfirm::getName() const { return "CONFIRM"; }

TClntMsgConfirm::~TClntMsgConfirm() {}
