/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 */

#include "ClntOptTimeZone.h"
#include "ClntMsg.h"
#include "Logger.h"
#include "OptDUID.h"

TClntOptTimeZone::TClntOptTimeZone(const std::string &domain, TMsg *parent)
    : TOptString(OPTION_NEW_TZDB_TIMEZONE, domain, parent) {}

TClntOptTimeZone::TClntOptTimeZone(char *buf, int bufsize, TMsg *parent)
    : TOptString(OPTION_NEW_TZDB_TIMEZONE, buf, bufsize, parent) {
  /// @todo: do some validity check
}

bool TClntOptTimeZone::isValid() const {
  /// @todo: check is somehow
  return true;
}

bool TClntOptTimeZone::doDuties() {
  std::string reason = "trying to set time zone.";
  int ifindex = Parent->getIface();

  SPtr<TOptDUID> duid = SPtr_cast<TOptDUID>(Parent->getOption(OPTION_SERVERID));
  if (!duid) {
    Log(Error) << "Unable to find proper DUID while " << reason << LogEnd;
    return false;
  }

  SPtr<TClntIfaceIface> iface = SPtr_cast<TClntIfaceIface>(ClntIfaceMgr().getIfaceByID(ifindex));
  if (!iface) {
    Log(Error) << "Unable to find interface ifindex=" << ifindex << reason << LogEnd;
    return false;
  }

  SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(ifindex);
  cfgIface->setTimezoneState(STATE_CONFIGURED);

  return iface->setTimezone(duid->getDUID(), Parent->getRemoteAddr(), Str);
}

/// @todo remove this
void TClntOptTimeZone::setSrvDuid(SPtr<TDUID> duid) { SrvDUID = duid; }
