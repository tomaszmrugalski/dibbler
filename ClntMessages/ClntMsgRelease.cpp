/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * chanmges: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRelease.cpp,v 1.9 2006-10-06 00:43:28 thomson Exp $
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
 * create RELEASE emssage
 * 
 * @param IfaceMgr 
 * @param TransMgr 
 * @param CfgMgr 
 * @param AddrMgr 
 * @param iface 
 * @param addr 
 * @param iaLst - TAddrIA list, which are served by on server on one link
 * @param ta 
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
    IRT=REL_TIMEOUT;
    MRT=0;
    MRC=REL_MAX_RC;
    MRD=0;
    RT=0;

    SmartPtr<TAddrIA> ia = 0;
    if (iaLst.count()) {
	iaLst.first();
	ia=iaLst.get();
    }
    if (!ia)
	ia = ta;
    if (!ia) {
	Log(Error) << "Unable to send RELEASE. No IA and no TA provided." << LogEnd;
	this->IsDone = true;
	return;
    }
    if (!ia->getDUID()) {
	Log(Error) << "Unable to send RELEASE. Unable to find DUID. " << LogEnd;
    }

    Options.append(new TClntOptServerIdentifier( ia->getDUID(),this));
    Options.append(new TClntOptClientIdentifier( CfgMgr->getDUID(),this));
    iaLst.first();
    while(ia=iaLst.get()) {
        Options.append(new TClntOptIA_NA(ia,this));
	SmartPtr<TAddrAddr> ptrAddr;
	SmartPtr<TClntIfaceIface> ptrIface;
	ptrIface = (Ptr*)IfaceMgr->getIfaceByID(ia->getIface());
	ia->firstAddr();
	while (ptrAddr = ia->getAddr()) {
	    ptrIface->delAddr(ptrAddr->get());
	    Log(Notice)<< ptrAddr->get()->getPlain() 
		       << " address released from " << ptrIface->getName() << "/" 
		       << ptrIface->getID() << " interface." << LogEnd;
	}

	// --- DNS Update ---
	SPtr<TIPv6Addr> dns = ia->getFQDNDnsServer();
	if (dns) {
	    string fqdn = ptrIface->getFQDN();
	    IfaceMgr->fqdnDel(ptrIface, ia, fqdn);
	}
	// --- DNS Update ---
    }
    
    if (ta)
	Options.append(new TClntOptTA(ta, this));

    // prefix delegation release

    SmartPtr<TAddrIA> pd = 0;
    
    pdLst.first();
    while(pd=pdLst.get()) {
        Options.append(new TClntOptIA_PD(pd,this));
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
}
