/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntAddrMgr.cpp,v 1.4 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "SmartPtr.h"
#include "AddrIA.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "AddrClient.h"
#include "Logger.h"


TClntAddrMgr::TClntAddrMgr(SmartPtr<TClntCfgMgr> ClntCfgMgr, string addrdb, bool loadDB)
    :TAddrMgr(addrdb, loadDB)
{
    
    // add this client (with proper duid)
    SmartPtr<TAddrClient> client = new TAddrClient(ClntCfgMgr->getDUID());
    addClient(client);

    // set Client field
    firstClient();
    Client = getClient();
}


long TClntAddrMgr::getT1Timeout()
{
    return Client->getT1Timeout();
}
long TClntAddrMgr::getT2Timeout()
{
    return Client->getT2Timeout();
}
long TClntAddrMgr::getPrefTimeout()
{
    return Client->getPrefTimeout();
}
long TClntAddrMgr::getValidTimeout()
{
    return Client->getValidTimeout();
}

unsigned long TClntAddrMgr::getTimeout()
{
    unsigned long val,val2;
    val = this->getT1Timeout();
    if ( (val2 = this->getT2Timeout()) < val)
	val = val2;
    if ( (val2 = this->getPrefTimeout()) < val)
	val = val2;
    if ( (val2 = this->getValidTimeout()) < val)
	val = val2;
    return val;
}

unsigned long TClntAddrMgr::getTentativeTimeout()
{
    SmartPtr<TAddrIA> ptrIA;
    Client->firstIA();
    unsigned long min = DHCPV6_INFINITY;
    unsigned long tmp;
//    std::clog << logger::logDebug << "getTentativeTimeout(): ";
    while(ptrIA=Client->getIA())
    {
	tmp = ptrIA->getTentativeTimeout();
//	std::clog << " IA(IAID=" << ptrIA->getIAID() << "),timeout=" << tmp;
	if (min > tmp)
	    min = tmp;
    }
//    std::clog << logger::endl;
    return min;
}

/* removes outdated addrs from addrDB
   sends DECLINE for addrs which are detected as tentative
   removes addrs from IfaceMgr 
 */
void TClntAddrMgr::doDuties()
{
    SmartPtr<TAddrIA> ptrIA;
    SmartPtr<TAddrAddr> ptrAddr;

    //Will contain list of ia to be refreshed
    TContainer<SmartPtr<TAddrIA> > IALst;
    Client->firstIA();
    while ( ptrIA = Client->getIA() ) 
    {
	while( ptrAddr=ptrIA->getAddr())
	{
	    //Removing outdated addresses
	    if(!ptrAddr->getValidTimeout())
	    {
		//Here shoulb be also removed from iface manager
		ptrIA->delAddr(ptrAddr->get());
	    }
	    
	}
    }
	    
}

void TClntAddrMgr::firstIA()
{
    Client->firstIA();
}

SmartPtr<TAddrIA> TClntAddrMgr::getIA()
{
    return Client->getIA();
}

bool TClntAddrMgr::delIA(long IAID)
{
    return Client->delIA(IAID);
}

void TClntAddrMgr::addIA(SmartPtr<TAddrIA> ptr)
{
    Client->addIA(ptr);
}

int TClntAddrMgr::countIA()
{
    return Client->countIA();
}




TClntAddrMgr::~TClntAddrMgr()
{
}

bool TClntAddrMgr::isIAAssigned(long IAID)
{
	SmartPtr<TAddrIA> ia;
	//FIXME:maybe it should be also method of TAddrMgr ? (Marek)
	firstIA();
	while (ia=getIA())
		if (ia->getIAID()==IAID) return true;
	return false;
}	

SmartPtr<TAddrIA> TClntAddrMgr::getIA(long IAID)
{
    SmartPtr<TAddrIA> ptrIA;
    this->Client->firstIA();
    while (ptrIA = this->Client->getIA() ) {
	if (ptrIA->getIAID() == IAID)
	    return ptrIA;
    }
    return SmartPtr<TAddrIA>();
}
