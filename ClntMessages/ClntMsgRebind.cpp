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

#include "ClntMsgRebind.h"
#include "AddrIA.h"
#include "ClntMsg.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIA_PD.h"
#include "Logger.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "SmartPtr.h"
#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

TClntMsgRebind::TClntMsgRebind(TOptList ptrOpts, int iface)
    : TClntMsg(iface, SPtr<TIPv6Addr>(), REBIND_MSG) {
  Options = ptrOpts;
  IRT = REB_TIMEOUT;
  MRT = REB_MAX_RT;
  MRC = 0;
  RT = 0;

  // there are options copied from RENEW. Get rid of some of them
  delOption(OPTION_ELAPSED_TIME);
  delOption(OPTION_SERVERID);
  SPtr<TOpt> opt;

  // calculate timeout (how long should be the REBIND message transmitted)
  unsigned long maxMRD = 0;
  firstOption();
  while (opt = getOption()) {
    switch (opt->getOptType()) {
      case OPTION_IA_NA: {
        SPtr<TClntOptIA_NA> ptrIA = SPtr_cast<TClntOptIA_NA>(opt);
        SPtr<TAddrIA> ptrAddrIA = ClntAddrMgr().getIA(ptrIA->getIAID());
        if (ptrAddrIA && maxMRD < ptrAddrIA->getMaxValidTimeout()) {
          maxMRD = ptrAddrIA->getMaxValidTimeout();
        }
        break;
      }
      case OPTION_IA_PD: {
        SPtr<TClntOptIA_PD> pd = SPtr_cast<TClntOptIA_PD>(opt);
        SPtr<TAddrIA> addrPd = ClntAddrMgr().getPD(pd->getIAID());
        if (addrPd && maxMRD < addrPd->getMaxValidTimeout()) {
          maxMRD = addrPd->getMaxValidTimeout();
        }
        break;
      }
    }
  }
  MRD = maxMRD;

  appendElapsedOption();
  appendAuthenticationOption();

  IsDone = false;
  send();
}

void TClntMsgRebind::answer(SPtr<TClntMsg> Reply) {
  TClntMsg::answer(Reply);
  return;

#if 0
  /// @todo: Fix REPLY support for REBIND
  
SPtr<TOpt> opt;
    
    // get DUID
    SPtr<TClntOptServerIdentifier> ptrDUID;
    ptrDUID = Reply->getOption(OPTION_SERVERID);
    
    Reply->firstOption();
    // for each option in message... (there should be only one IA option, as we send
    // separate RENEW for each IA, but we check all options anyway)
    while ( opt = Reply->getOption() ) {
        switch (opt->getOptType()) {
        case OPTION_IA_NA: {
            iaCnt++;
            SPtr<TClntOptIA_NA> ptrOptIA = opt;
            if (ptrOptIA->getStatusCode()!=STATUSCODE_SUCCESS) {
                if(ptrOptIA->getStatusCode() == STATUSCODE_NOBINDING){
                    ClntTransMgr().sendRequest(Options,Iface);
                    IsDone = true;
                    return;
                }else{
		    SPtr<TOptStatusCode> status = ptrOptIA->getOption(OPTION_STATUS_CODE);
                    Log(Warning) << "Received IA (iaid=" << ptrOptIA->getIAID() << ") with status code " <<
                        StatusCodeToString(status->getCode()) << ": "
                                 << status->getText() << LogEnd;
                    break;
                }
            }
            ptrOptIA->setContext(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr,
                                 ptrDUID->getDUID(), SPtr<TIPv6Addr>() /*NULL*/, Reply->getIface());

            ptrOptIA->doDuties();
            break;
        }
        case OPTION_IA_PD: {
            iaCnt++;
            SPtr<TClntOptIA_PD> pd = opt;
            if (pd->getStatusCode() != STATUSCODE_SUCCESS) {
                if(pd->getStatusCode() == STATUSCODE_NOBINDING){
                    ClntTransMgr->sendRequest(Options,Iface);
                    IsDone = true;
                    return;
                }
                else{
                    SPtr<TClntOptStatusCode> status = pd->getOption(OPTION_STATUS_CODE);
                    Log(Warning) << "Received PD (iaid=" << pd->getIAID() << ") with status code " <<
                        StatusCodeToString(status->getCode()) << ": "
                                 << status->getText() << LogEnd;
                    break;
                }
            }
            pd->setContext(ClntIfaceMgr, ClntTransMgr, ClntCfgMgr, ClntAddrMgr, ptrDUID->getDUID(), 0, (TMsg*)this);
            pd->doDuties();
            break;
        }
        case OPTION_ORO:
        case OPTION_RELAY_MSG:
        case OPTION_INTERFACE_ID:
        case OPTION_IAADDR:
        case OPTION_RECONF_MSG:
            Log(Warning) << "Illegal option (" << opt->getOptType()
                         << ") in received REPLY message." << LogEnd;
            break;
        default:
            // what to do with unknown/other options? execute them
            opt->setParent(this);
            opt->doDuties();
        }
    }
    //Here we received answer from our server, which updated the "whole information"
    //There is no use to send Rebind even if server realesed some addresses/IAs
    //in such a case new Solicit message should be sent
 
   if(iaCnt) IsDone = true;
#endif
}

void TClntMsgRebind::updateIA(SPtr<TClntOptIA_NA> ptrOptIA, SPtr<TOptDUID> optSrvDUID,
                              SPtr<TOptAddr> optUnicast) {

  SPtr<TAddrIA> ptrAddrIA;
  bool found = false;

  // ..find IA in addrMgr...
  ClntAddrMgr().firstIA();
  while (ptrAddrIA = ClntAddrMgr().getIA()) {
    if (ptrOptIA->getIAID() == ptrAddrIA->getIAID()) {
      found = true;
      break;
    }
  }

  if (found) {

    // set new server's DUID to handle this IA
    ptrAddrIA->setDUID(optSrvDUID->getDUID());

    //
    if (optUnicast) {
      ptrAddrIA->setUnicast(optUnicast->getAddr());
    } else {
      ptrAddrIA->setMulticast();
    }

    // IAID found, set up new received options.
    SPtr<TAddrAddr> ptrAddrAddr;
    SPtr<TOptIAAddress> ptrOptAddr;

    // are all addrs configured?
    if (ptrOptIA->countAddr() != ptrAddrIA->countAddr()) {
      this->releaseIA(ptrOptIA->getIAID());
    }

    // for each address in IA option...
    ptrOptIA->firstAddr();
    while (ptrOptAddr = ptrOptIA->getAddr()) {
      ptrAddrAddr = ptrAddrIA->getAddr(ptrOptAddr->getAddr());
      if (!ptrAddrAddr) {
        // there is no such addr in db
        /// @todo: what to do with new addrs?
        // (Thomson:I think we should RELEASE all addrs, and issue new REQUEST
        // for this IA)
      } else {
        if ((ptrOptAddr->getPref() == 0) || (ptrOptAddr->getValid() == 0)) {
          releaseIA(ptrOptIA->getIAID());
          break;  // analyze next option OPTION_IA_NA
        }
        // set up new options
        if (ptrOptAddr->getPref() != ptrAddrAddr->getPref()) {
          // received diffrent prefered-lifetime
          /// @todo:
        }
        if (ptrOptAddr->getValid() != ptrAddrAddr->getValid()) {
          // received diffrent prefered-lifetime
          /// @todo:
        }
        ptrAddrAddr->setTimestamp();
      }
    }
    ptrAddrIA->setT1(ptrOptIA->getT1());
    ptrAddrIA->setT2(ptrOptIA->getT2());
    ptrAddrIA->setTimestamp();
    ptrAddrIA->setState(STATE_CONFIGURED);
  } else {
    // unknown IAID, ignore it
    Log(Warning) << "Received message contains unknown IA (IAID=" << ptrOptIA->getIAID()
                 << "). Ignoring it." << LogEnd;
  }
}

void TClntMsgRebind::doDuties() {
  SPtr<TIfaceIface> iface = ClntIfaceMgr().getIfaceByID(this->Iface);

  if (!MRD) {
    ostringstream iaLst;
    ostringstream pdLst;
    SPtr<TOpt> ptrOpt;
    firstOption();
    while (ptrOpt = getOption()) {
      switch (ptrOpt->getOptType()) {
        case OPTION_IA_NA: {
          SPtr<TClntOptIA_NA> ptrIA = SPtr_cast<TClntOptIA_NA>(ptrOpt);
          iaLst << ptrIA->getIAID() << " ";
          releaseIA(ptrIA->getIAID());
          break;
        }
        case OPTION_IA_PD: {
          SPtr<TClntOptIA_PD> ptrPD = SPtr_cast<TClntOptIA_PD>(ptrOpt);
          ptrPD->setContext(SPtr<TDUID>(), SPtr<TIPv6Addr>(), this);
          ptrPD->delPrefixes();
          break;
        }
      };
    }
    Log(Warning) << "REBIND for the IA(s):" << iaLst.str() << ", PD(s):" << pdLst.str()
                 << " failed on the " << iface->getFullName() << " interface." << LogEnd;
    Log(Warning) << "Restarting server discovery process." << LogEnd;
    IsDone = true;
  } else
    this->send();
}

void TClntMsgRebind::releaseIA(int IAID) {
  SPtr<TAddrIA> ptrAddrIA = ClntAddrMgr().getIA(IAID);
  if (!ptrAddrIA) {
    Log(Error) << "IA has not been found in Address Manager." << LogEnd;
    return;
  }

  SPtr<TAddrAddr> ptrAddr;
  ptrAddrIA->firstAddr();
  while (ptrAddr = ptrAddrIA->getAddr()) {
    // remove outdated address from interface
    SPtr<TIfaceIface> ptrIface = ClntIfaceMgr().getIfaceByID(ptrAddrIA->getIfindex());
    if (!ptrIface) {
      Log(Error) << "Unable to find interface with ifindex " << ptrAddrIA->getIfindex() << LogEnd;
      continue;
    }
    ptrIface->delAddr(ptrAddr->get(), ptrIface->getPrefixLength());
    // and from db
    ptrAddrIA->delAddr(ptrAddr->get());
  }
  ptrAddrIA->setState(STATE_NOTCONFIGURED);

  SPtr<TClntCfgIA> cfgIA = ClntCfgMgr().getIA(IAID);
  if (!cfgIA) return;
  cfgIA->setState(STATE_NOTCONFIGURED);
}

bool TClntMsgRebind::check() { return 0; }

std::string TClntMsgRebind::getName() const { return "REBIND"; }

TClntMsgRebind::~TClntMsgRebind() {}
