/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRelease.cpp,v 1.7 2006-08-21 22:22:52 thomson Exp $
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
	SmartPtr<TAddrIA> ta)
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
	SmartPtr<TIfaceIface> ptrIface;
	ptrIface = IfaceMgr->getIfaceByID(ia->getIface());
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
	    // let's do deleting update
	    SmartPtr<TIPv6Addr> clntAddr;
	    ia->firstAddr();
	    SPtr<TAddrAddr> tmpAddr = ia->getAddr();
	    SPtr<TIPv6Addr> myAddr = tmpAddr->get();

	    SmartPtr<TClntIfaceIface> ptrIface = (Ptr*)ClntIfaceMgr->getIfaceByID(iface);
	    string fqdn = ptrIface->getFQDN();
  		
	    Log(Debug) << "FQDN: Cleaning up DNS AAAA record in server " << *dns << ", for IP=" << *myAddr
		       << " and FQDN=" << fqdn << LogEnd;
	    unsigned int dotpos = fqdn.find(".");
	    string hostname = "";
	    string domain = "";
	    if (dotpos == string::npos) {
		Log(Warning) << "FQDN: Name provided for DNS update is not a FQDN. [" << fqdn
			     << "] Trying to do the cleanup..." << LogEnd;
		hostname = fqdn;
	    }
	    else {
		hostname = fqdn.substr(0, dotpos);
		domain = fqdn.substr(dotpos + 1, fqdn.length() - dotpos - 1);
	    }
	    
	    Log(Debug) << "FQDN: Domain name set to " << (char*) domain.c_str() << "." << LogEnd;
#ifndef MOD_CLNT_DISABLE_DNSUPDATE
	    DNSUpdate *act = new DNSUpdate(dns->getPlain(), (char*) domain.c_str(), (char*) hostname.c_str(), 
					   myAddr->getPlain(), "2h",4);
	    int result = act->run();
	    delete act;
	    
	    if (result == DNSUPDATE_SUCCESS) {
		
		Log(Notice) << "FQDN: delete procedure succesful." << LogEnd;
	    } else {
		Log(Warning) << "Unable to perform DNS update. Clean up Disabling FQDN on " 
			     << ptrIface->getFullName() << LogEnd;
		
	    }
#else
	    Log(Error) << "This Dibbler version is compiled without DNS Update support." << LogEnd;
#endif
	}
	// --- DNS Update ---
    }
    
    if (ta)
	Options.append(new TClntOptTA(ta, this));

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
