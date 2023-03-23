/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Krzysztof Wnuk <keczi@poczta.onet.pl>
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "ClntOptIA_PD.h"
#include "AddrIA.h"
#include "ClntCfgMgr.h"
#include "ClntIfaceMgr.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "OptIAPrefix.h"
#include "OptStatusCode.h"
#include "Portable.h"

using namespace std;

/**
 * Used in REQUEST, RENEW, REBIND, DECLINE and RELEASE
 *
 * @param addrPD
 * @param parent
 */
TClntOptIA_PD::TClntOptIA_PD(SPtr<TAddrIA> addrPD, TMsg *parent)
    : TOptIA_PD(addrPD->getIAID(), addrPD->getT1(), addrPD->getT2(), parent),
      Unicast(false),
      Iface(-1) {

  bool zeroTimes = false;
  if ((parent->getType() == RELEASE_MSG) || (parent->getType() == DECLINE_MSG)) {
    T1_ = 0;
    T2_ = 0;
    zeroTimes = true;
  }

  clearContext();

  SPtr<TAddrPrefix> ptrPrefix;
  addrPD->firstPrefix();
  while (ptrPrefix = addrPD->getPrefix()) {
    SubOptions.append(new TOptIAPrefix(ptrPrefix->get(), ptrPrefix->getLength(),
                                       zeroTimes ? 0 : ptrPrefix->getPref(),
                                       zeroTimes ? 0 : ptrPrefix->getValid(), this->Parent));
  }
}

/// @brief constructor used in building SOLICIT message
///
/// @param cfgPD
/// @param parent
TClntOptIA_PD::TClntOptIA_PD(SPtr<TClntCfgPD> cfgPD, TMsg *parent)
    : TOptIA_PD(cfgPD->getIAID(), cfgPD->getT1(), cfgPD->getT2(), parent),
      Unicast(false),
      Iface(-1) {
  cfgPD->firstPrefix();
  SPtr<TClntCfgPrefix> cfgPrefix;
  while (cfgPrefix = cfgPD->getPrefix()) {
    SubOptions.append(new TOptIAPrefix(cfgPrefix->get(), cfgPrefix->getLength(),
                                       cfgPrefix->getPref(), cfgPrefix->getValid(), 0));
  }
  clearContext();
}

/**
 * Used to create object from received message
 *
 * @param buf
 * @param bufsize
 * @param parent
 */
TClntOptIA_PD::TClntOptIA_PD(char *buf, int bufsize, TMsg *parent)
    : TOptIA_PD(buf, bufsize, parent), Unicast(false) {
  Valid = parseOptions(SubOptions, buf, bufsize, parent, IAID_, "IA_PD option");
  clearContext();
}

void TClntOptIA_PD::firstPrefix() { SubOptions.first(); }

SPtr<TOptIAPrefix> TClntOptIA_PD::getPrefix() {
  SPtr<TOpt> ptr;
  do {
    ptr = SubOptions.get();
    if (ptr && ptr->getOptType() == OPTION_IAPREFIX) {
      return SPtr_cast<TOptIAPrefix>(ptr);
    }
  } while (ptr);
  return SPtr<TOptIAPrefix>();
}

int TClntOptIA_PD::countPrefix() const {
  const TOptList &opts = SubOptions.getSTL();

  int count = 0;
  for (TOptList::const_iterator opt = opts.begin(); opt != opts.end(); ++opt) {
    if ((*opt)->getOptType() != OPTION_IAPREFIX) continue;
    count++;
  }

  return count;
}

int TClntOptIA_PD::getStatusCode() {
  SPtr<TOpt> status = getOption(OPTION_STATUS_CODE);
  if (status) {
    SPtr<TOptStatusCode> statOpt = SPtr_cast<TOptStatusCode>(status);
    if (statOpt) {
      return statOpt->getCode();
    }
  }
  return STATUSCODE_SUCCESS;
}

void TClntOptIA_PD::setContext(SPtr<TDUID> srvDuid, SPtr<TIPv6Addr> srvAddr, TMsg *originalMsg) {
  DUID = srvDuid;
  if (srvAddr) {
    Unicast = true;
  } else {
    Unicast = false;
  }
  Prefix = srvAddr;
  OriginalMsg = originalMsg;
  Iface = originalMsg->getIface();
}

TClntOptIA_PD::~TClntOptIA_PD() {}

bool TClntOptIA_PD::doDuties() {
  if (!OriginalMsg) {
    Log(Error) << "Internal error. Unable to set prefixes: setContext() not called." << LogEnd;
    return false;
  }

  switch (OriginalMsg->getType()) {
    case REQUEST_MSG:
    case SOLICIT_MSG:
      return addPrefixes();
    case RELEASE_MSG:
      return delPrefixes();
    case RENEW_MSG:
    case REBIND_MSG:
      return updatePrefixes();
    default:
      break;
  }

  return true;
}

void TClntOptIA_PD::deletePrefix(SPtr<TOptIAPrefix> prefix) {
  TOptList &opts = SubOptions.getSTL();

  for (TOptList::iterator opt = opts.begin(); opt != opts.end(); ++opt) {
    if ((*opt)->getOptType() == OPTION_IAPREFIX) {
      SPtr<TOptIAPrefix> iaprefix = SPtr_cast<TOptIAPrefix>(*opt);
      if ((*prefix->getPrefix()) == (*iaprefix->getPrefix())) {
        opts.erase(opt);
        break;
      }
    }
  }
}

SPtr<TOptIAPrefix> TClntOptIA_PD::getPrefix(SPtr<TIPv6Addr> prefix) {
  TOptList &opts = SubOptions.getSTL();

  for (TOptList::iterator opt = opts.begin(); opt != opts.end(); ++opt) {
    if ((*opt)->getOptType() != OPTION_IAPREFIX) continue;
    SPtr<TOptIAPrefix> iaprefix = SPtr_cast<TOptIAPrefix>(*opt);

    if ((*prefix) == (*iaprefix->getPrefix())) return iaprefix;
  }
  return SPtr<TOptIAPrefix>();  // NULL
}

bool TClntOptIA_PD::addPrefixes() { return modifyPrefixes(TClntIfaceMgr::PREFIX_MODIFY_ADD); }

bool TClntOptIA_PD::delPrefixes() { return modifyPrefixes(TClntIfaceMgr::PREFIX_MODIFY_DEL); }

bool TClntOptIA_PD::updatePrefixes() { return modifyPrefixes(TClntIfaceMgr::PREFIX_MODIFY_UPDATE); }

bool TClntOptIA_PD::modifyPrefixes(TClntIfaceMgr::PrefixModifyMode mode) {
  bool status = false;
  EState state = STATE_NOTCONFIGURED;
  SPtr<TOptIAPrefix> prefix;
  string action;
  switch (mode) {
    case TClntIfaceMgr::PREFIX_MODIFY_ADD:
      action = "addition";
      state = STATE_CONFIGURED;
      break;
    case TClntIfaceMgr::PREFIX_MODIFY_UPDATE:
      action = "update";
      state = STATE_CONFIGURED;
      break;
    case TClntIfaceMgr::PREFIX_MODIFY_DEL:
      action = "delete";
      state = STATE_NOTCONFIGURED;
      break;
  }

  if ((mode == TClntIfaceMgr::PREFIX_MODIFY_ADD) || (mode == TClntIfaceMgr::PREFIX_MODIFY_UPDATE)) {
    if ((T1_ == 0) && (T2_ == 0)) {
      TOptList &opts = SubOptions.getSTL();
      for (TOptList::iterator it = opts.begin(); it != opts.end(); ++it) {
        if ((*it)->getOptType() != OPTION_IAPREFIX) continue;
        prefix = SPtr_cast<TOptIAPrefix>(*it);
        T1_ = prefix->getPref() / 2;
        T2_ = (int)((prefix->getPref()) * 0.7);
        Log(Notice) << "Server set T1 and T2 to 0. Choosing default (50%, "
                       "70% * prefered-lifetime): T1="
                    << T1_ << ", T2=" << T2_ << LogEnd;
      }
    }
  }

  SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(this->Iface);
  if (!cfgIface) {
    Log(Error) << "Unable to set PD state for iaid=" << getIAID() << " received on interface "
               << "ifindex=" << Iface << ": No such interface in CfgMgr found." << LogEnd;
    return false;
  }

  TNotifyScriptParams *scriptParams = 0;
  if (OriginalMsg) {
    scriptParams = static_cast<TNotifyScriptParams *>(OriginalMsg->getNotifyScriptParams());
  }

  this->firstPrefix();
  while (prefix = this->getPrefix()) {
    switch (mode) {
      case TClntIfaceMgr::PREFIX_MODIFY_ADD:
        ClntAddrMgr().addPrefix(this->DUID, this->Prefix, cfgIface->getName(), this->Iface, IAID_,
                                T1_, T2_, prefix->getPrefix(), prefix->getPref(),
                                prefix->getValid(), prefix->getPrefixLength(), false);
        status =
            ClntIfaceMgr().addPrefix(this->Iface, prefix->getPrefix(), prefix->getPrefixLength(),
                                     prefix->getPref(), prefix->getValid(), scriptParams);
        Log(Debug) << "RENEW(IA_PD) will be sent (T1) after " << T1_ << ", REBIND (T2) after "
                   << T2_ << " seconds." << LogEnd;
        action = "addition";
        break;
      case TClntIfaceMgr::PREFIX_MODIFY_UPDATE:
        ClntAddrMgr().updatePrefix(this->DUID, this->Prefix, cfgIface->getName(), this->Iface,
                                   IAID_, T1_, T2_, prefix->getPrefix(), prefix->getPref(),
                                   prefix->getValid(), prefix->getPrefixLength(), false);
        status =
            ClntIfaceMgr().updatePrefix(this->Iface, prefix->getPrefix(), prefix->getPrefixLength(),
                                        prefix->getPref(), prefix->getValid(), scriptParams);
        Log(Debug) << "RENEW(IA_PD) will be sent (T1) after " << T1_ << ", REBIND (T2) after "
                   << T2_ << " seconds." << LogEnd;
        action = "update";
        break;
      case TClntIfaceMgr::PREFIX_MODIFY_DEL:
        ClntAddrMgr().delPrefix(ClntCfgMgr().getDUID(), IAID_, prefix->getPrefix(), false);
        status = ClntIfaceMgr().delPrefix(this->Iface, prefix->getPrefix(),
                                          prefix->getPrefixLength(), scriptParams);
        action = "delete";
        break;
    }

    if (!status) {
      string tmp = error_message();
      Log(Error) << "Prefix error encountered during prefix " << action << " operation: " << tmp
                 << LogEnd;
      // Let's pretend it was configured and renew it
      // setState(STATE_FAILED);
      // return true;
    }
  }

  setState(state);
  return true;
}

void TClntOptIA_PD::setIface(int iface) { this->Iface = iface; }

bool TClntOptIA_PD::isValid() const {
  const TOptList &opts = SubOptions.getSTL();

  for (TOptList::const_iterator it = opts.begin(); it != opts.end(); ++it) {
    if ((*it)->getOptType() != OPTION_IAPREFIX) continue;
    SPtr<TOptIAPrefix> prefix = SPtr_cast<TOptIAPrefix>(*it);

    if (prefix && prefix->getPrefix()->linkLocal()) {
      Log(Warning) << "Prefix " << prefix->getPrefix()->getPlain()
                   << " used in IA_PD (IAID=" << IAID_
                   << ") is link local. The whole IA option is considered invalid." << LogEnd;
      return false;
    }
  }

  // Last paragraph in section 9 of RFC3633
  // If a requesting router receives an IA_PD with T1 greater than T2, and
  // both T1 and T2 are greater than 0, the client discards the IA_PD
  // option and processes the remainder of the message as though the
  // delegating router had not included the IA_PD option.
  if (T1_ > T2_) {
    Log(Warning) << "Received malformed IA_PD: T1(" << T1_ << ") is greater than T2(" << T2_
                 << "). Ignoring IA_PD with iaid=" << IAID_ << LogEnd;
    return false;
  }

  return Valid;
}

void TClntOptIA_PD::setState(EState state) {
  SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(this->Iface);
  if (!cfgIface) {
    Log(Error) << "Unable to set PD state for iaid=" << getIAID() << " received on interface "
               << "ifindex=" << Iface << ": No such interface in CfgMgr found." << LogEnd;
    return;
  }

  SPtr<TClntCfgPD> cfgPD = cfgIface->getPD(getIAID());
  if (!cfgPD) {
    Log(Error) << "Unable to find PD with iaid=" << getIAID() << " on the "
               << cfgIface->getFullName() << " interface (CfgMgr)." << LogEnd;
    return;
  }
  cfgPD->setState(state);

  SPtr<TAddrIA> addrPD = ClntAddrMgr().getPD(getIAID());
  if (!addrPD) {
    /* Log(Error) << "Unable to find PD with iaid=" << getIAID() << " on the "
       << cfgIface->getFullName() << " interface (AddrMgr)." << LogEnd; */
    /* Don't complain about it. It is normal that IA is being deleted when there
       are no more prefixes in it */
    return;
  }
  addrPD->setState(state);
}

void TClntOptIA_PD::clearContext() {
  DUID.reset();
  OriginalMsg = 0;
}
