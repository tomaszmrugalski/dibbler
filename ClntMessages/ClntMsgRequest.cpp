/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgRequest.cpp,v 1.29 2009-03-24 23:17:17 thomson Exp $
 *
 */

#include "ClntMsgRequest.h"
#include "ClntIfaceMgr.h"
#include "ClntMsgAdvertise.h"
#include "ClntOptElapsed.h"
#include "ClntOptIA_NA.h"
#include "ClntOptIA_PD.h"
#include "ClntOptTA.h"
#include "ClntTransMgr.h"
#include "DHCPConst.h"
#include "Logger.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "OptStatusCode.h"
#include "SmartPtr.h"
#include <cmath>

/*
 * opts - options list WITHOUT serverDUID
 */
TClntMsgRequest::TClntMsgRequest(TOptList opts, int iface)
    : TClntMsg(iface, SPtr<TIPv6Addr>(), REQUEST_MSG) {
  IRT = REQ_TIMEOUT;
  MRT = REQ_MAX_RT;
  MRC = REQ_MAX_RC;
  MRD = 0;
  RT = 0;

  Iface = iface;
  IsDone = false;

  int backupCount = ClntTransMgr().getAdvertiseLstCount();
  if (!backupCount) {
    Log(Error) << "Unable to send REQUEST. There are no backup servers left." << LogEnd;
    setState(opts, STATE_NOTCONFIGURED);

    this->IsDone = true;
    return;
  }
  Log(Info) << "Creating REQUEST. Backup server list contains " << backupCount << " server(s)."
            << LogEnd;
  ClntTransMgr().printAdvertiseLst();

  // get server DUID from the first advertise
  SPtr<TOpt> srvDUID = ClntTransMgr().getAdvertiseDUID();

  ClntTransMgr().firstAdvertise();
  SPtr<TClntMsg> advertise = SPtr_cast<TClntMsg>(ClntTransMgr().getAdvertise());
  copyAAASPI(SPtr_cast<TClntMsg>(advertise));

  // remove just used server
  ClntTransMgr().delFirstAdvertise();

  // copy whole list from SOLICIT ...
  Options = opts;

  // set proper Parent in copied options
  TOptList::iterator opt = Options.begin();
  while (opt != Options.end()) {
    if ((*opt)->getOptType() == OPTION_ELAPSED_TIME)
      opt = Options.erase(opt);
    else {
      (*opt)->setParent(this);
      ++opt;
    }
  }

  // copy addresses offered in ADVERTISE
  copyAddrsFromAdvertise(advertise);

  copyPrefixesFromAdvertise(advertise);

  // does this server support unicast?
  SPtr<TClntCfgIface> cfgIface = ClntCfgMgr().getIface(iface);
  if (!cfgIface) {
    Log(Error) << "Unable to find interface with ifindex " << iface << "." << LogEnd;
    IsDone = true;
    return;
  }
  SPtr<TOptAddr> unicast = SPtr_cast<TOptAddr>(advertise->getOption(OPTION_UNICAST));
  if (unicast && !cfgIface->getUnicast()) {
    Log(Info) << "Server offers unicast (" << *unicast->getAddr()
              << ") communication, but this client is not configured to so." << LogEnd;
  }
  if (unicast && cfgIface->getUnicast()) {
    Log(Debug) << "Server supports unicast on address " << *unicast->getAddr() << "." << LogEnd;

    // this->PeerAddr = unicast->getAddr();
    firstOption();
    SPtr<TOpt> opt;
    // set this unicast address in each IA in AddrMgr
    while (opt = getOption()) {
      /// @todo: add support for unicast in IA_TA and IA_PD
      if (opt->getOptType() != OPTION_IA_NA) continue;
      SPtr<TClntOptIA_NA> ptrOptIA = SPtr_cast<TClntOptIA_NA>(opt);
      SPtr<TAddrIA> ptrAddrIA = ClntAddrMgr().getIA(ptrOptIA->getIAID());

      if (!ptrAddrIA) {
        Log(Crit) << "IA with IAID=" << ptrOptIA->getIAID() << " not found." << LogEnd;
        continue;
      }
      ptrAddrIA->setUnicast(unicast->getAddr());
    }
  }

  // ... and append server's DUID from ADVERTISE
  Options.push_back(srvDUID);

  appendElapsedOption();
  appendAuthenticationOption();
  IsDone = false;
  send();
}

TClntMsgRequest::TClntMsgRequest(List(TAddrIA) IAs, SPtr<TDUID> srvDUID, int iface)
    : TClntMsg(iface, SPtr<TIPv6Addr>(), REQUEST_MSG) {
  IRT = REQ_TIMEOUT;
  MRT = REQ_MAX_RT;
  MRC = REQ_MAX_RC;
  MRD = 0;
  RT = 0;

  Iface = iface;
  IsDone = false;
  SPtr<TOpt> ptr;
  ptr = new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(), this);
  Options.push_back(ptr);

  if (!srvDUID) {
    Log(Error) << "Unable to send REQUEST: ServerId not specified.\n" << LogEnd;
    IsDone = true;
    return;
  }

  ptr = new TOptDUID(OPTION_SERVERID, srvDUID, this);
  // all IAs provided by checkSolicit
  SPtr<TAddrIA> ClntAddrIA;
  Options.push_back(ptr);

  IAs.first();
  while (ClntAddrIA = IAs.get()) {
    SPtr<TClntCfgIA> ClntCfgIA = ClntCfgMgr().getIA(ClntAddrIA->getIAID());
    SPtr<TOpt> ia = new TClntOptIA_NA(ClntCfgIA, ClntAddrIA, this);
    Options.push_back(ia);
  }

  appendElapsedOption();
  appendAuthenticationOption();

  this->IsDone = false;
  this->send();
}

/*
 * analyse REPLY received for this REQUEST
 */
void TClntMsgRequest::answer(SPtr<TClntMsg> msg) {
  this->copyAAASPI(msg);

#ifdef MOD_CLNT_CONFIRM
  /* CHANGED here: When the client receives a NotOnLink status from the server
   * in response to a Request, the client can either re-issue the Request
   * without specifying any addresses or restart the DHCP server discovery
   * process.
   */
  SPtr<TOptStatusCode> optStateCode = msg->getStatusCode();
  if (optStateCode && (STATUSCODE_NOTONLINK == optStateCode->getCode())) {
    SPtr<TOpt> opt;
    firstOption();
    while (opt = getOption()) {
      if (opt->getOptType() != OPTION_IA_NA) {
        continue;
      }
      opt->delOption(OPTION_IAADDR);
    }
    return;
  }
#endif

  TClntMsg::answer(msg);
}

void TClntMsgRequest::doDuties() {
  // timeout is reached and we still don't have answer, retransmit
  if (RC > MRC) {
    ClntTransMgr().sendRequest(Options, Iface);

    IsDone = true;
    return;
  }
  send();
  return;
}

bool TClntMsgRequest::check() { return false; }

std::string TClntMsgRequest::getName() const { return "REQUEST"; }

/**
 * method is used when REQUEST transmission was attempted, but there are no more
 * servers on the backup list. When this method is called, it is sure that some
 * IAs, TA or PDs will remain not configured (as there are no servers available,
 * which could configure it)
 *
 * @param options list of still unconfigured options
 * @param state state to be set in CfgMgr
 */
void TClntMsgRequest::setState(TOptList options, EState state) {
  SPtr<TOpt> opt;

  SPtr<TClntCfgIface> iface = ClntCfgMgr().getIface(Iface);
  if (!iface) {
    Log(Error) << "Unable to find interface with ifindex=" << Iface << LogEnd;
    return;
  }

  for (TOptList::iterator opt = options.begin(); opt != options.end(); ++opt) {
    switch ((*opt)->getOptType()) {
      case OPTION_IA_NA: {
        SPtr<TClntOptIA_NA> ia = SPtr_cast<TClntOptIA_NA>(*opt);
        if (!ia) {
          continue;
        }
        SPtr<TClntCfgIA> cfgIa = iface->getIA(ia->getIAID());
        if (cfgIa) cfgIa->setState(state);
        break;
      }
      case OPTION_IA_TA: {
        iface->firstTA();
        SPtr<TClntCfgTA> cfgTa = iface->getTA();
        if (cfgTa) cfgTa->setState(state);
        break;
      }
      case OPTION_IA_PD: {
        SPtr<TClntOptIA_PD> pd = SPtr_cast<TClntOptIA_PD>(*opt);
        SPtr<TClntCfgPD> cfgPd = iface->getPD(pd->getIAID());
        if (cfgPd) cfgPd->setState(state);
        break;
      }
      default:
        continue;
    }
  }
}

void TClntMsgRequest::copyAddrsFromAdvertise(SPtr<TClntMsg> adv) {
  if (ClntCfgMgr().insistMode())
    return;  // insist-mode on, so ask for the address user specified again

  SPtr<TOpt> opt1, opt2, opt3;
  SPtr<TClntOptIA_NA> ia1, ia2;

  this->copyAAASPI(adv);

  firstOption();
  while (opt1 = getOption()) {
    if (opt1->getOptType() != OPTION_IA_NA) continue;  // ignore all options except IA_NA
    adv->firstOption();
    while (opt2 = adv->getOption()) {
      if (opt2->getOptType() != OPTION_IA_NA) continue;
      ia1 = SPtr_cast<TClntOptIA_NA>(opt1);
      ia2 = SPtr_cast<TClntOptIA_NA>(opt2);
      if (!ia1 || !ia2 || (ia1->getIAID() != ia2->getIAID())) continue;

      // found IA in ADVERTISE, now copy all addrs

      ia1->delAllOptions();
      ia2->firstOption();
      while (opt3 = ia2->getOption()) {
        if (opt3->getOptType() == OPTION_IAADDR) ia1->addOption(opt3);
      }
    }
  }
}

void TClntMsgRequest::copyPrefixesFromAdvertise(SPtr<TClntMsg> adv) {
  if (ClntCfgMgr().insistMode())
    return;  // insist-mode on, so ask for the address user specified again

  SPtr<TOpt> opt1, opt2, opt3;
  SPtr<TClntOptIA_PD> ia1, ia2;

  this->copyAAASPI(adv);

  firstOption();
  while (opt1 = getOption()) {
    if (opt1->getOptType() != OPTION_IA_PD) continue;  // ignore all options except IA_NA
    adv->firstOption();
    while (opt2 = adv->getOption()) {
      if (opt2->getOptType() != OPTION_IA_PD) continue;
      ia1 = SPtr_cast<TClntOptIA_PD>(opt1);
      ia2 = SPtr_cast<TClntOptIA_PD>(opt2);
      if (!ia1 || !ia2 || (ia1->getIAID() != ia2->getIAID())) continue;

      // found IA_PD in ADVERTISE, now copy all addrs
      ia1->delAllOptions();
      ia2->firstOption();
      while (opt3 = ia2->getOption()) {
        if (opt3->getOptType() == OPTION_IAPREFIX) ia1->addOption(opt3);
      }
    }
  }
}

TClntMsgRequest::~TClntMsgRequest() {}
