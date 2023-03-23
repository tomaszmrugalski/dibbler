/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntOptIA_NA.h"
#include "AddrIA.h"
#include "ClntAddrMgr.h"
#include "ClntCfgIA.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"
#include "Msg.h"
#include "OptAddrParams.h"
#include "OptStatusCode.h"

/**
 * Used in CONFIRM constructor
 *
 * @param clntAddrIA
 * @param zeroTimes
 * @param parent
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TAddrIA> clntAddrIA, bool zeroTimes, TMsg *parent)
    : TOptIA_NA(clntAddrIA->getIAID(), zeroTimes ? 0 : clntAddrIA->getT1(),
                zeroTimes ? 0 : clntAddrIA->getT2(), parent),
      Iface_(0) {
  SPtr<TAddrAddr> addr;
  clntAddrIA->firstAddr();
  while (addr = clntAddrIA->getAddr()) {
    SubOptions.append(new TOptIAAddress(addr->get(), zeroTimes ? 0 : addr->getPref(),
                                        zeroTimes ? 0 : addr->getValid(), parent));
  }
}

/**
 * Used in DECLINE, RENEW and RELEASE
 *
 * @param addrIA
 * @param parent
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TAddrIA> addrIA, TMsg *parent)
    : TOptIA_NA(addrIA->getIAID(), addrIA->getT1(), addrIA->getT2(), parent) {
  // should we include all addrs or tentative ones only?
  bool decline;
  if (parent->getType() == DECLINE_MSG)
    decline = true;
  else
    decline = false;

  bool zeroTimes = false;
  if ((parent->getType() == RELEASE_MSG) || (parent->getType() == DECLINE_MSG)) {
    T1_ = 0;
    T2_ = 0;
    zeroTimes = true;
  }

  SPtr<TAddrAddr> ptrAddr;
  addrIA->firstAddr();
  while (ptrAddr = addrIA->getAddr()) {
    if (!decline || (ptrAddr->getTentative() == ADDRSTATUS_YES))
      SubOptions.append(new TOptIAAddress(ptrAddr->get(), zeroTimes ? 0 : ptrAddr->getPref(),
                                          zeroTimes ? 0 : ptrAddr->getValid(), this->Parent));
  }
  DUID = SPtr<TDUID>();  // NULL
}

/**
 * Used in REQUEST constructor
 *
 * @param ClntCfgIA
 * @param ClntaddrIA
 * @param parent
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TClntCfgIA> ClntCfgIA, SPtr<TAddrIA> ClntaddrIA, TMsg *parent)
    : TOptIA_NA(ClntaddrIA->getIAID(), ClntaddrIA->getT1(), ClntaddrIA->getT2(), parent) {
  // checkRequestConstructor
  ClntCfgIA->firstAddr();
  SPtr<TClntCfgAddr> ClntCfgAddr;

  /// @todo: keep allocated address in TAddrClient
  while ((ClntCfgAddr = ClntCfgIA->getAddr()) &&
         ((ClntCfgIA->countAddr() - ClntaddrIA->countAddr()) > this->countAddr())) {
    SPtr<TAddrAddr> ptrAddr = ClntaddrIA->getAddr(ClntCfgAddr->get());
    if (!ptrAddr)
      SubOptions.append(new TOptIAAddress(ClntCfgAddr->get(), ClntCfgAddr->getPref(),
                                          ClntCfgAddr->getValid(), parent));
  }
  DUID = SPtr<TDUID>();  // NULL
}

/**
 * Used in SOLICIT constructor
 *
 * @param ClntCfgIA
 * @param parent
 */
TClntOptIA_NA::TClntOptIA_NA(SPtr<TClntCfgIA> ClntCfgIA, TMsg *parent)
    : TOptIA_NA(ClntCfgIA->getIAID(), ClntCfgIA->getT1(), ClntCfgIA->getT2(), parent) {
  ClntCfgIA->firstAddr();
  SPtr<TClntCfgAddr> ClntCfgAddr;
  // just copy all addresses defined in the CfgMgr
  while (ClntCfgAddr = ClntCfgIA->getAddr())
    SubOptions.append(new TOptIAAddress(ClntCfgAddr->get(), ClntCfgAddr->getPref(),
                                        ClntCfgAddr->getValid(), parent));
}

/**
 * Used to create object from received message
 *
 * @param buf
 * @param bufsize
 * @param parent
 */
TClntOptIA_NA::TClntOptIA_NA(char *buf, int bufsize, TMsg *parent)
    : TOptIA_NA(buf, bufsize, parent) {
  Valid = parseOptions(SubOptions, buf, bufsize, parent, IAID_, "IA_NA option");
}

void TClntOptIA_NA::firstAddr() { SubOptions.first(); }

SPtr<TOptIAAddress> TClntOptIA_NA::getAddr() {
  SPtr<TOpt> ptr;
  do {
    ptr = SubOptions.get();
    if (ptr && ptr->getOptType() == OPTION_IAADDR) {
      return SPtr_cast<TOptIAAddress>(ptr);
    }
  } while (ptr);
  return SPtr<TOptIAAddress>();
}

int TClntOptIA_NA::countAddr() {
  SPtr<TOpt> ptr;
  SubOptions.first();
  int count = 0;
  while (ptr = SubOptions.get()) {
    if (ptr->getOptType() == OPTION_IAADDR) count++;
  }
  return count;
}

int TClntOptIA_NA::getStatusCode() {
  SPtr<TOpt> status = getOption(OPTION_STATUS_CODE);
  if (status) {
    SPtr<TOptStatusCode> statOpt = SPtr_cast<TOptStatusCode>(status);
    if (statOpt) {
      return statOpt->getCode();
    }
  }
  return STATUSCODE_SUCCESS;
}

void TClntOptIA_NA::setContext(SPtr<TDUID> srvDuid, SPtr<TIPv6Addr> srvAddr, int iface) {
  DUID = srvDuid;
  Addr = srvAddr;
  Iface_ = iface;
}

TClntOptIA_NA::~TClntOptIA_NA() {}

bool TClntOptIA_NA::doDuties() {

  // find this IA in addrMgr...
  SPtr<TAddrIA> ptrIA = ClntAddrMgr().getIA(this->getIAID());
  if (!ptrIA) {
    // unknown IAID, ignore it
    Log(Warning) << "Received message contains unknown IA (IAID=" << this->getIAID()
                 << "). We didn't order it. Weird... ignoring it." << LogEnd;
    return true;
  }

  // IAID found, set up new received options.
  SPtr<TAddrAddr> ptrAddrAddr;
  SPtr<TOptIAAddress> ptrOptAddr;

  SPtr<TIfaceIface> ptrIface;
  ptrIface = ClntIfaceMgr().getIfaceByID(Iface_);
  if (!ptrIface) {
    Log(Error) << "Interface with ifindex=" << Iface_ << " not found." << LogEnd;
    return true;
  }

  // for each address in IA option...
  this->firstAddr();
  while (ptrOptAddr = this->getAddr()) {
    ptrAddrAddr = ptrIA->getAddr(ptrOptAddr->getAddr());

    // no such address in DB ??
    if (!ptrAddrAddr) {
      if (ptrOptAddr->getValid()) {

        int prefixLen = ptrIface->getPrefixLength();
        if (ptrOptAddr->getOption(OPTION_ADDRPARAMS)) {
          Log(Debug) << "Experimental addr-params found." << LogEnd;
          SPtr<TOptAddrParams> optAddrParams =
              SPtr_cast<TOptAddrParams>(ptrOptAddr->getOption(OPTION_ADDRPARAMS));
          prefixLen = optAddrParams->getPrefix();
        }

        // add this address in addrDB...
        ptrIA->addAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid(),
                       prefixLen);
        ptrIA->setDUID(this->DUID);

        // ... and in IfaceMgr -
        ptrIface->addAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid(),
                          prefixLen);
      } else {
        Log(Warning) << "Server send new addr with valid lifetime 0." << LogEnd;
      }
    } else {
      // we have this addr in DB
      if (ptrOptAddr->getValid() == 0) {
        // valid=0, release this address
        // delete address from addrDB
        ptrIA->delAddr(ptrOptAddr->getAddr());
        // delete address from IfaceMgr
        ptrIface->delAddr(ptrOptAddr->getAddr(), ptrIface->getPrefixLength());
        break;  // analyze next option OPTION_IA_NA
      }

      // set up new options in IfaceMgr
      SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(Iface_);
      if (ptrIface)
        ptrIface->updateAddr(ptrOptAddr->getAddr(), ptrOptAddr->getPref(), ptrOptAddr->getValid());
      // set up new options in addrDB
      ptrAddrAddr->setPref(ptrOptAddr->getPref());
      ptrAddrAddr->setValid(ptrOptAddr->getValid());
      ptrAddrAddr->setTimestamp();
    }
  }
  SPtr<TClntCfgIA> ptrCfgIA;
  ptrCfgIA = ClntCfgMgr().getIA(ptrIA->getIAID());

  if (getT1() && getT2()) {
    ptrIA->setT1(this->getT1());
    ptrIA->setT2(this->getT2());
    Log(Debug) << "RENEW(IA_NA) will be sent (T1) after " << ptrIA->getT1()
               << ", REBIND (T2) after " << ptrIA->getT2() << " seconds." << LogEnd;
  } else {
    this->firstAddr();
    ptrOptAddr = this->getAddr();
    if (ptrOptAddr) {
      ptrIA->setT1(ptrOptAddr->getPref() / 2);
      ptrIA->setT2((int)((ptrOptAddr->getPref()) * 0.7));
      Log(Notice) << "Server set T1 and T2 to 0. Choosing default (50%, "
                  << "70% * prefered-lifetime): T1=" << ptrIA->getT1() << ", T2=" << ptrIA->getT2()
                  << LogEnd;
    }
  }

  ptrIA->setTimestamp();
  ptrIA->setState(STATE_CONFIGURED);
  ptrCfgIA->setState(STATE_CONFIGURED);
  return true;
}

// Counts all valid and diffrent addresses in sum of
// addresses received in IA from server and addresses contained
// in its counterpart IA in address manager
int TClntOptIA_NA::countValidAddrs(SPtr<TAddrIA> ptrIA) {
  int count = 0;

  // Counts addresses not received in Reply message
  // but which are in addrDB, and are still valid (valid>0)
  SPtr<TAddrAddr> ptrAddr;
  ptrIA->firstAddr();
  // For each addr in DB for this IA
  while (ptrAddr = ptrIA->getAddr()) {
    // If addr is still valid
    if (!ptrAddr->getValid()) continue;
    // and is not included in received option
    if (!this->getAddr(ptrAddr->get()))
      // we can increase counter
      count++;
  }

  // count all valid (valid>0) addresses received from server
  /// @todo: A) check if they repeats (possible with maliciious server)
  //       B) and not already assigned in addrDB (in others IAs)
  //       It's easy and worth checking
  //       A) Create list of valid addrs and add new valid addr
  //          only if it doesn't exist already on this list
  //       B) Address can be assigned only in this IA, not in other
  //          This could be ommited if VerifyIA worked prooperly
  this->firstAddr();
  SPtr<TOptIAAddress> ptrOptIAAddr;
  while (ptrOptIAAddr = this->getAddr()) {
    if (!ptrOptIAAddr->getValid()) continue;
    // if (!ptrIA->getAddr(ptrOptIAAddr->getAddr()))
    count++;
  }
  return count;
}

SPtr<TOptIAAddress> TClntOptIA_NA::getAddr(SPtr<TIPv6Addr> addr) {
  SPtr<TOptIAAddress> optAddr;
  this->firstAddr();
  while (optAddr = this->getAddr()) {
    //! memcmp(optAddr->getAddr(),addr,16)
    if ((*addr) == (*optAddr->getAddr())) return optAddr;
  };
  return SPtr<TOptIAAddress>();
}

void TClntOptIA_NA::releaseAddr(long IAID, SPtr<TIPv6Addr> addr) {
  SPtr<TAddrIA> ptrIA = ClntAddrMgr().getIA(IAID);
  if (ptrIA)
    ptrIA->delAddr(addr);
  else
    Log(Warning) << "Unable to release addr: IA (" << IAID << ") not present in addrDB." << LogEnd;
}

void TClntOptIA_NA::setIface(int iface) { Iface_ = iface; }

bool TClntOptIA_NA::isValid() const {

  const TOptList &opts = SubOptions.getSTL();

  for (TOptList::const_iterator it = opts.begin(); it != opts.end(); ++it) {
    if ((*it)->getOptType() != OPTION_IAADDR) continue;
    SPtr<TOptIAAddress> addr = SPtr_cast<TOptIAAddress>(*it);
    if (addr && addr->getAddr()->linkLocal()) {
      Log(Warning) << "Address " << addr->getAddr()->getPlain() << " used in IA_NA (IAID=" << IAID_
                   << ") is link local. The whole IA option is considered invalid." << LogEnd;
      return false;
    }
  }

  // RFC3315, section 22.4:
  // If a client receives an IA_NA with T1 greater than T2, and both T1
  // and T2 are greater than 0, the client discards the IA_NA option and
  // processes the remainder of the message as though the server had not
  // included the invalid IA_NA option.
  if (T1_ > T2_) {
    Log(Warning) << "Received malformed IA_NA: T1(" << T1_ << ") is greater than T2(" << T2_
                 << "). Ignoring IA_NA." << LogEnd;
    return false;
  }

  return Valid;
}
