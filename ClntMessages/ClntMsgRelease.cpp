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
 */

#include "ClntMsgRelease.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "AddrIA.h"
#include "OptDUID.h"
#include "ClntCfgMgr.h"
#include "ClntOptIA_NA.h"
#include "ClntOptTA.h"
#include "ClntOptIA_PD.h"
#include <cmath>
#include "Logger.h"
#include "ClntAddrMgr.h"
#include "AddrMgr.h"
#include "AddrIA.h"
#include "AddrAddr.h"

using namespace std;

/**
 * create RELEASE message
 *
 * @param iface
 * @param addr
 * @param iaLst - IA_NA list, which are served by on server on one link
 * @param ta    - IA_TA to be released
 * @param pdLst - IA_PD list to be released
 */
TClntMsgRelease::TClntMsgRelease(int iface, SPtr<TIPv6Addr> addr,
                                 List(TAddrIA) iaLst,
                                 SPtr<TAddrIA> ta,
                                 List(TAddrIA) pdLst)
  :TClntMsg(iface, addr, RELEASE_MSG)
{
    SPtr<TDUID> srvDUID;

    IRT=REL_TIMEOUT;
    MRT=0;
    MRC=REL_MAX_RC;
    MRD=0;
    RT=0;

    // obtain IA, TA or PD, so server DUID can be obtained
    SPtr<TAddrIA> x;
    if (iaLst.count()) {
        iaLst.first();
        x = iaLst.get();
    }
    if (!x)
        x = ta;
    if (!x) {
        pdLst.first();
        x = pdLst.get();
    }
    if (!x) {
        Log(Error) << "Unable to send RELEASE. No IA, TA or PD provided." << LogEnd;
        this->IsDone = true;
        return;
    }
    if (!x->getDUID()) {
        Log(Error) << "Unable to send RELEASE. Unable to find DUID. " << LogEnd;
        this->IsDone = true;
        return;
    }
    srvDUID = x->getDUID();

    Options.push_back(new TOptDUID(OPTION_SERVERID, srvDUID,this));
    Options.push_back(new TOptDUID(OPTION_CLIENTID, ClntCfgMgr().getDUID(),this));

    // --- RELEASE IA ---
    iaLst.first();
    while(x=iaLst.get()) {
        Options.push_back(new TClntOptIA_NA(x,this));
        SPtr<TAddrAddr> ptrAddr;
        SPtr<TClntIfaceIface> ptrIface;
        ptrIface = (Ptr*)ClntIfaceMgr().getIfaceByID(x->getIfindex());
        if (!ptrIface) {
            Log(Warning) << "Unable to find interface with ifindex "
                         << x->getIfindex() << " while creating RELEASE." << LogEnd;
            continue;
        }
        x->firstAddr();
        while (ptrAddr = x->getAddr()) {
            ptrIface->delAddr(ptrAddr->get(), ptrAddr->getPrefix());
        }

        // --- DNS Update ---
        SPtr<TIPv6Addr> dns = x->getFQDNDnsServer();
        if (dns) {
            string fqdn = ptrIface->getFQDN();
            ClntIfaceMgr().fqdnDel(ptrIface, x, fqdn);
        }
        // --- DNS Update ---
    }

    // --- RELEASE TA ---
    if (ta) {
        Options.push_back(new TClntOptTA(ta, this));
        SPtr<TClntIfaceIface> ptrIface;
        ptrIface = (Ptr*)ClntIfaceMgr().getIfaceByID(ta->getIfindex());
        if (ptrIface) {
            SPtr<TAddrAddr> addr;
            ta->firstAddr();
            while (addr = ta->getAddr()) {
                ptrIface->delAddr(addr->get(), addr->getPrefix());
            }
        } else {
            Log(Warning) << "Unable to find interface with ifindex "
                         << ta->getIfindex() << " while creating RELEASE." << LogEnd;
        }
    }

    // --- RELEASE PD ---

    SPtr<TAddrIA> pd;

    pdLst.first();
    while(pd=pdLst.get()) {
        SPtr<TClntOptIA_PD> pdOpt = new TClntOptIA_PD(pd,this);
        Options.push_back( (Ptr*)pdOpt );
        pdOpt->setContext(srvDUID, addr, this);
        pdOpt->delPrefixes();

        ClntAddrMgr().delPD(pd->getIAID() );
    }

    appendElapsedOption();
    appendAuthenticationOption();

    IsDone = false;
    send();
}

void TClntMsgRelease::answer(SPtr<TClntMsg> rep)
{
    SPtr<TOptDUID> rspSrvID = (Ptr*) rep->getOption(OPTION_SERVERID);
    SPtr<TOptDUID> msgSrvID = (Ptr*) getOption(OPTION_SERVERID);
    if (!rspSrvID) {
        Log(Warning) << "Received reply to RELEASE without SERVER-ID option." << LogEnd;
    }

    if (!msgSrvID) {
        Log(Error) << "Sent RELEASE did not contain SERVER-ID. That seems to be my fault. Sorry." << LogEnd;
    }

    if (rspSrvID && msgSrvID &&
        (!(*msgSrvID->getDUID()==*rspSrvID->getDUID()))) {
        Log(Error) << "Internal error. RELEASE sent to server with DUID=" << msgSrvID->getPlain()
                   << ", but response returned with " << rspSrvID->getPlain() << LogEnd;
    }

    IsDone = true; // we don't care. We sent release, we are shutting down anyway.
}

void TClntMsgRelease::doDuties() {
    if (RC!=MRC)
        send();
    else
       IsDone=true;
    return;
}

bool TClntMsgRelease::check() {
    return false;
}

std::string TClntMsgRelease::getName() const {
    return "RELEASE";
}

TClntMsgRelease::~TClntMsgRelease() {
}
