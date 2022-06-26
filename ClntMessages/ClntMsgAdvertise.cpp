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

#include "ClntMsgAdvertise.h"
#include "ClntOptPreference.h"
#include "OptDUID.h"
#include "OptInteger.h"
#include <sstream>

using namespace std;

/*
 * creates buffer based on buffer
 */
TClntMsgAdvertise::TClntMsgAdvertise(int iface, SPtr<TIPv6Addr> addr, char *buf, int buflen)
    : TClntMsg(iface, addr, buf, buflen) {}

bool TClntMsgAdvertise::check() {
  return TClntMsg::check(true /* clientID mandatory */, true /* serverID mandatory */);
}

int TClntMsgAdvertise::getPreference() {
  SPtr<TOptInteger> ptr;
  ptr = SPtr_cast<TOptInteger>(getOption(OPTION_PREFERENCE));
  if (!ptr) {
    return 0;
  }
  return ptr->getValue();
}

void TClntMsgAdvertise::answer(SPtr<TClntMsg> Rep) {
  // this should never happen
}

void TClntMsgAdvertise::doDuties() {
  // this should never happen
}

string TClntMsgAdvertise::getName() const { return "ADVERTISE"; }

string TClntMsgAdvertise::getInfo() {
  ostringstream tmp;
  SPtr<TOptDUID> srvID = getServerID();

  int pref = getPreference();

  if (srvID) {
    tmp << "Server ID=" << srvID->getDUID()->getPlain();
  } else {
    tmp << "malformed (Server ID option missing)";
  }

  tmp << ", preference=" << pref;

  return tmp.str();
}

TClntMsgAdvertise::~TClntMsgAdvertise() {}
