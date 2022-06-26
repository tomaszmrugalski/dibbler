/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntMsgReply.h"
#include "OptIAAddress.h"
#include "SmartPtr.h"

TClntMsgReply::TClntMsgReply(int iface, SPtr<TIPv6Addr> addr, char *buf, int bufSize)
    : TClntMsg(iface, addr, buf, bufSize) {}

void TClntMsgReply::answer(SPtr<TClntMsg> Reply) {
  // this should never happen. After receiving REPLY for e.g. REQUEST,
  // request->answer(reply) is called. Client nevers sends reply msg, so
  // this method will never be called.
}

void TClntMsgReply::doDuties() {}

bool TClntMsgReply::check() {
  bool anonInfReq = ClntCfgMgr().anonInfRequest();
  return TClntMsg::check(!anonInfReq /* clientID mandatory */, true /* serverID mandatory */);
}

std::string TClntMsgReply::getName() const { return "REPLY"; }

SPtr<TIPv6Addr> TClntMsgReply::getFirstAddr() {
  firstOption();
  SPtr<TOpt> opt, subopt;
  while (opt = getOption()) {
    if (opt->getOptType() != OPTION_IA_NA) continue;
    opt->firstOption();
    while (subopt = opt->getOption()) {
      if (subopt->getOptType() != OPTION_IAADDR) continue;
      SPtr<TOptIAAddress> optAddr = SPtr_cast<TOptIAAddress>(subopt);
      if (optAddr) {
        return optAddr->getAddr();
      }
    }
  }

  return SPtr<TIPv6Addr>();  // NULL
}

TClntMsgReply::~TClntMsgReply() {}
