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

#include "ClntMsgDecline.h"
#include "AddrIA.h"
#include "ClntOptIA_NA.h"
#include "OptDUID.h"

TClntMsgDecline::TClntMsgDecline(int iface, SPtr<TIPv6Addr> addr, List(TAddrIA) IALst)
    : TClntMsg(iface, addr, DECLINE_MSG) {
  IRT = DEC_TIMEOUT;
  MRT = 0;
  // these both below mean there is no ending condition and transactions
  // lasts till receiving answer
  MRC = DEC_MAX_RC;
  MRD = 0;
  RT = 0;

  // include our ClientIdentifier
  SPtr<TOpt> ptr;
  ptr = new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this);
  Options.push_back(ptr);

  // include server's DUID
  ptr = new TOptDUID(OPTION_SERVERID, IALst.getFirst()->getDUID(), this);
  Options.push_back(ptr);

  // create IAs
  SPtr<TAddrIA> ptrIA;
  IALst.first();
  while (ptrIA = IALst.get()) {
    Options.push_back(new TClntOptIA_NA(ptrIA, this));
  }

  appendElapsedOption();
  appendAuthenticationOption();
  IsDone = false;
  send();
}

void TClntMsgDecline::answer(SPtr<TClntMsg> rep) {
  /// @todo: Is UseMulticast option included?

  SPtr<TOptDUID> repSrvID = rep->getServerID();
  SPtr<TOptDUID> msgSrvID = getServerID();
  if ((!repSrvID) || !msgSrvID || (!(*msgSrvID->getDUID() == *repSrvID->getDUID()))) {
    return;
  }
  IsDone = true;
}

void TClntMsgDecline::doDuties() {
  if (RC != MRC)
    send();
  else
    IsDone = true;
  return;
}

bool TClntMsgDecline::check() { return false; }

std::string TClntMsgDecline::getName() const { return "DECLINE"; }

TClntMsgDecline::~TClntMsgDecline() {}
