/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "ClntOptTA.h"
#include "AddrIA.h"
#include "ClntAddrMgr.h"
#include "ClntIfaceMgr.h"
#include "Logger.h"
#include "OptDUID.h"
#include "OptIAAddress.h"
#include "OptStatusCode.h"

/**
 * Constructor used during
 *
 * @param iaid
 * @param parent
 */
TClntOptTA::TClntOptTA(unsigned int iaid, TMsg *parent) : TOptTA(iaid, parent), Iface(-1) {
  // don't put any suboptions
}

/**
 * Constructor used during option reception
 *
 * @param buf
 * @param bufsize
 * @param parent
 */
TClntOptTA::TClntOptTA(char *buf, int bufsize, TMsg *parent)
    : TOptTA(buf, bufsize, parent), Iface(-1) {
  Valid = parseOptions(SubOptions, buf, bufsize, parent, IAID_, "IA_TA option");
}

TClntOptTA::TClntOptTA(SPtr<TAddrIA> ta, TMsg *parent) : TOptTA(ta->getIAID(), parent), Iface(-1) {
  ta->firstAddr();
  SPtr<TAddrAddr> addr;
  while (addr = ta->getAddr()) {
    SubOptions.append(new TOptIAAddress(addr->get(), 0, 0, parent));
  }
}

void TClntOptTA::firstAddr() { SubOptions.first(); }

SPtr<TOptIAAddress> TClntOptTA::getAddr() {
  SPtr<TOpt> ptr;
  do {
    ptr = SubOptions.get();

    if (ptr && ptr->getOptType() == OPTION_IAADDR) {
      return (SPtr_cast<TOptIAAddress>(ptr));
    }
  } while (ptr);

  return SPtr<TOptIAAddress>();  // NULL
}

int TClntOptTA::countAddr() {
  SPtr<TOpt> ptr;
  SubOptions.first();
  int count = 0;
  while (ptr = SubOptions.get()) {
    if (ptr->getOptType() == OPTION_IAADDR) count++;
  }
  return count;
}

int TClntOptTA::getStatusCode() {
  SPtr<TOpt> status = getOption(OPTION_STATUS_CODE);
  if (status) {
    SPtr<TOptStatusCode> statOpt = SPtr_cast<TOptStatusCode>(status);
    if (statOpt) {
      return statOpt->getCode();
    }
  }
  return STATUSCODE_SUCCESS;
}

TClntOptTA::~TClntOptTA() {}

/**
 * This function sets everything in motion, i.e. sets up temporary addresses
 *
 * @return
 */
bool TClntOptTA::doDuties() {
  // find this TA in addrMgr...
  SPtr<TAddrIA> ta = ClntAddrMgr().getTA(this->getIAID());

  SPtr<TClntCfgIface> cfgIface;
  if (!(cfgIface = ClntCfgMgr().getIface(this->Iface))) {
    Log(Error) << "Unable to find TA class in the CfgMgr, on the " << this->Iface << " interface."
               << LogEnd;
    return true;
  }

  SPtr<TOptDUID> duid = SPtr_cast<TOptDUID>(Parent->getOption(OPTION_SERVERID));
  if (!duid) {
    Log(Error) << "Unable to find proper DUID while setting TA." << LogEnd;
    return false;
  }

  if (!ta) {
    Log(Debug) << "Creating TA (iaid=" << this->getIAID() << ") in the addrDB." << LogEnd;
    ta = new TAddrIA(cfgIface->getName(), this->Iface, IATYPE_TA,
                     SPtr<TIPv6Addr>() /*if unicast, then this->Addr*/, duid->getDUID(),
                     DHCPV6_INFINITY, DHCPV6_INFINITY, this->getIAID());
    ClntAddrMgr().addTA(ta);
  }

  // IAID found, set up new received options.
  SPtr<TAddrAddr> addr;
  SPtr<TOptIAAddress> optAddr;

  SPtr<TIfaceIface> ptrIface;
  ptrIface = ClntIfaceMgr().getIfaceByID(this->Iface);
  if (!ptrIface) {
    Log(Error) << "Interface " << this->Iface << " not found." << LogEnd;
    return true;
  }

  // for each address in IA option...
  this->firstAddr();
  while (optAddr = this->getAddr()) {
    addr = ta->getAddr(optAddr->getAddr());

    if (!addr) {  // - no such address in DB -
      if (!optAddr->getValid()) {
        Log(Warning) << "Server send new addr with valid=0." << LogEnd;
        continue;
      }
      // add this address in addrDB...
      ta->addAddr(optAddr->getAddr(), optAddr->getPref(), optAddr->getValid());
      ta->setDUID(duid->getDUID());
      // ... and in IfaceMgr -
      ptrIface->addAddr(optAddr->getAddr(), optAddr->getPref(), optAddr->getValid(),
                        ptrIface->getPrefixLength());
      Log(Notice) << "Temp. address " << *optAddr->getAddr() << " has been added to "
                  << ptrIface->getName() << "/" << ptrIface->getID() << " interface." << LogEnd;

    } else {  // - we have this addr in DB -

      if (optAddr->getValid() == 0) {
        // valid=0, release this address and delete address from addrDB
        ta->delAddr(optAddr->getAddr());
        // delete address from IfaceMgr
        ptrIface->delAddr(optAddr->getAddr(), ptrIface->getPrefixLength());
        continue;  // analyze next option OPTION_IA
      }

      // set up new options in IfaceMgr
      ptrIface->updateAddr(optAddr->getAddr(), optAddr->getPref(), optAddr->getValid());
      // set up new options in addrDB
      addr->setPref(optAddr->getPref());
      addr->setValid(optAddr->getValid());
      addr->setTimestamp();
    }
  }

  // mark this TA as configured
  SPtr<TClntCfgTA> cfgTA;
  cfgIface->firstTA();
  cfgTA = cfgIface->getTA();
  cfgTA->setState(STATE_CONFIGURED);

  ta->setState(STATE_CONFIGURED);
  return true;
}

SPtr<TOptIAAddress> TClntOptTA::getAddr(SPtr<TIPv6Addr> addr) {
  SPtr<TOptIAAddress> optAddr;
  this->firstAddr();
  while (optAddr = this->getAddr()) {
    //! memcmp(optAddr->getAddr(),addr,16)
    if ((*addr) == (*optAddr->getAddr())) return optAddr;
  };
  return SPtr<TOptIAAddress>();
}

void TClntOptTA::releaseAddr(long IAID, SPtr<TIPv6Addr> addr) {
  SPtr<TAddrIA> ptrIA = ClntAddrMgr().getIA(IAID);
  if (ptrIA)
    ptrIA->delAddr(addr);
  else
    Log(Warning) << "Unable to release addr: IA (" << IAID << ") not present in addrDB." << LogEnd;
}

bool TClntOptTA::isValid() const {
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

  return true;
}

void TClntOptTA::setContext(int iface, SPtr<TIPv6Addr> clntAddr) {
  this->Iface = iface;
  this->Addr = clntAddr;
}

void TClntOptTA::setIface(int iface) { this->Iface = iface; }
