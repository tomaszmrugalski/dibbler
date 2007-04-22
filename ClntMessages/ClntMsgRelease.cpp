/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chanmges: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRelease.cpp,v 1.15 2007-04-22 21:19:28 thomson Exp $
 */

#include "ClntMsgRelease.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "AddrIA.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptClientIdentifier.h"
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
#include "DNSUpdate.h"

/** 
 * create RELEASE message
 * 
 * @param IfaceMgr 
 * @param TransMgr 
 * @param CfgMgr 
 * @param AddrMgr 
 * @param iface 
 * @param addr 
 * @param iaLst - IA_NA list, which are served by on server on one link
 * @param ta    - IA_TA to be released
 * @param pdLst - IA_PD list to be released
 */
TClntMsgRelease::TClntMsgRelease(
	SmartPtr<TClntIfaceMgr> IfaceMgr, 
	SmartPtr<TClntTransMgr> TransMgr,
	SmartPtr<TClntCfgMgr>   CfgMgr, 
	SmartPtr<TClntAddrMgr>  AddrMgr, 
	int iface, SmartPtr<TIPv6Addr> addr,
	List(TAddrIA) iaLst,
	SmartPtr<TAddrIA> ta,
	List(TAddrIA) pdLst)
	:TClntMsg(IfaceMgr, TransMgr, CfgMgr, AddrMgr, iface, addr, RELEASE_MSG)
{
    SPtr<TDUID> srvDUID;
    
    IRT=REL_TIMEOUT;
    MRT=0;
    MRC=REL_MAX_RC;
    MRD=0;
    RT=0;

    // obtain IA, TA or PD, so server DUID can be obtained
    SmartPtr<TAddrIA> x = 0;
    if (iaLst.count()) {
	iaLst.first();
	x=iaLst.get();
    }
    if (!x)
	x = ta;
    if (!x) {
	pdLst.first();
	x = pdLst.get();
    }
    if (!x) {
	Log(Error) << "Unable to send RELEASE. No IA and no TA provided." << LogEnd;
	this->IsDone = true;
	return;
    }
    if (!x->getDUID()) {
	Log(Error) << "Unable to send RELEASE. Unable to find DUID. " << LogEnd;
    }
    srvDUID = x->getDUID();

    Options.append(new TClntOptServerIdentifier( srvDUID,this));
    Options.append(new TClntOptClientIdentifier( CfgMgr->getDUID(),this));

    // --- RELEASE IA ---
    iaLst.first();
    while(x=iaLst.get()) {
        Options.append(new TClntOptIA_NA(x,this));
	SmartPtr<TAddrAddr> ptrAddr;
	SmartPtr<TClntIfaceIface> ptrIface;
	ptrIface = (Ptr*)IfaceMgr->getIfaceByID(x->getIface());
	x->firstAddr();
	while (ptrAddr = x->getAddr()) {
	    ptrIface->delAddr(ptrAddr->get(), ptrAddr->getPrefix());
	}

	// --- DNS Update ---
	SPtr<TIPv6Addr> dns = x->getFQDNDnsServer();
	if (dns) {
	    string fqdn = ptrIface->getFQDN();
	    IfaceMgr->fqdnDel(ptrIface, x, fqdn);
	}
	// --- DNS Update ---
    }
    
    // --- RELEASE TA ---
    if (ta)
	Options.append(new TClntOptTA(ta, this));

    // --- RELEASE PD ---

    SmartPtr<TAddrIA> pd = 0;
    
    pdLst.first();
    while(pd=pdLst.get()) {
	SPtr<TClntOptIA_PD> pdOpt = new TClntOptIA_PD(pd,this);
        Options.append( (Ptr*)pdOpt);
	pdOpt->setContext(IfaceMgr, TransMgr, CfgMgr, AddrMgr, srvDUID, addr, this);
	pdOpt->delPrefixes();

	AddrMgr->delPD(pd->getIAID() );
    }

    pkt = new char[getSize()];
    IsDone = false;
}

void TClntMsgRelease::answer(SmartPtr<TClntMsg> rep)
{
    SmartPtr<TClntOptServerIdentifier> repSrvID= (Ptr*)  rep->getOption(OPTION_SERVERID);
    SmartPtr<TClntOptServerIdentifier> msgSrvID= (Ptr*)  this->getOption(OPTION_SERVERID);
    if ((!repSrvID)||
        (!(*msgSrvID->getDUID()==*repSrvID->getDUID())))
       return;
    IsDone=true;   
}

void TClntMsgRelease::doDuties()
{
    if (RC!=MRC)
       	send();
    else
       IsDone=true;
    return;
}

bool TClntMsgRelease::check()
{
	return false;
}

string TClntMsgRelease::getName() {
    return "RELEASE";
}

TClntMsgRelease::~TClntMsgRelease()
{
    delete [] pkt;
}
