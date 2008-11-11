/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntAddrMgr.cpp,v 1.26 2008-11-11 22:02:43 thomson Exp $
 */

#include "SmartPtr.h"
#include "AddrIA.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "AddrClient.h"
#include "Logger.h"


TClntAddrMgr::TClntAddrMgr(SmartPtr<TClntCfgMgr> ClntCfgMgr, string xmlFile, bool loadDB)
    :TAddrMgr(xmlFile, ClntCfgMgr->useConfirm())
{
    // client may have been already loaded from client-AddrMgr.xml file
    firstClient();
    if (!getClient()) {
	// add this client (with proper duid)
	SmartPtr<TAddrClient> client = new TAddrClient(ClntCfgMgr->getDUID());
	addClient(client);
    }

    // set Client field
    DeleteEmptyClient = false; // don't delete this client, even when IAs or PD has been removed
    firstClient();
    Client = getClient();
}


unsigned long TClntAddrMgr::getT1Timeout() {
    return Client->getT1Timeout();
}
unsigned long TClntAddrMgr::getT2Timeout() {
    return Client->getT2Timeout();
}
unsigned long TClntAddrMgr::getPrefTimeout() {
    return Client->getPrefTimeout();
}
unsigned long TClntAddrMgr::getValidTimeout() {
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

    while(ptrIA=Client->getIA())
    {
	tmp = ptrIA->getTentativeTimeout();
	if (min > tmp)
	    min = tmp;
    }

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

    this->firstIA();
    while ( ptrIA = this->getIA() ) 
    {
	ptrIA->firstAddr();
	while( ptrAddr=ptrIA->getAddr())
	{
	    //Removing outdated addresses
	    if(!ptrAddr->getValidTimeout())
	    {
		ptrIA->delAddr(ptrAddr->get());
		Log(Notice) << "Address " << ptrAddr->get()->getPlain() 
			     << " from IA " << ptrIA->getIAID() 
			     << " has been removed from addrDB." << LogEnd;
		ptrIA->setState(STATE_NOTCONFIGURED);
	    }
	}
    }
	    
}

void TClntAddrMgr::firstIA() {
    Client->firstIA();
}

SmartPtr<TAddrIA> TClntAddrMgr::getIA() {
    return Client->getIA();
}

bool TClntAddrMgr::delIA(long IAID) {
    return Client->delIA(IAID);
}

void TClntAddrMgr::addIA(SmartPtr<TAddrIA> ptr) {
    Client->addIA(ptr);
}

int TClntAddrMgr::countIA() {
    return Client->countIA();
}

TClntAddrMgr::~TClntAddrMgr() {
    this->Client = 0;
    Log(Debug) << "ClntAddrMgr cleanup." << LogEnd;
}

bool TClntAddrMgr::isIAAssigned(unsigned long IAID)
{
	SmartPtr<TAddrIA> ia;
	firstIA();
	while (ia=getIA())
		if (ia->getIAID()==IAID) return true;
	return false;
}	

SmartPtr<TAddrIA> TClntAddrMgr::getIA(unsigned long IAID)
{
    SmartPtr<TAddrIA> ptrIA;
    this->Client->firstIA();
    while (ptrIA = this->Client->getIA() ) {
	if (ptrIA->getIAID() == IAID)
	    return ptrIA;
    }
    return 0;
}

/** 
 * sets specified interface to CONFIRMME state
 * 
 * @param changedLinks structure containing interface indexes to be confirmed
 */
void TClntAddrMgr::setIA2Confirm(volatile link_state_notify_t * changedLinks)
{
    SmartPtr<TAddrIA> ptrIA;
    this->firstIA();
    while(ptrIA = this->getIA()){

	bool found = false;
	int ifindex = ptrIA->getIface(); // interface index of this IA

	// is this index on the list of interfaces to be confirmed?
	for (int i=0; i < MAX_LINK_STATE_CHANGES_AT_ONCE; i++)
	    if (changedLinks->ifindex[i]==ifindex)
		found = true;

	if (!found)
	    continue;

        if( (ptrIA->getState() == STATE_CONFIGURED || ptrIA->getState() == STATE_INPROCESS) )
	{
	    ptrIA->setState(STATE_CONFIRMME);
	    Log(Notice) << "Network switch off event detected. do Confirmming." << LogEnd;
	}
    } 

}

// pd functions 

void TClntAddrMgr::firstPD() {
    Client->firstPD();
}

SmartPtr<TAddrIA> TClntAddrMgr::getPD() {
    return Client->getPD();
}

bool TClntAddrMgr::delPD(long IAID) {
    return Client->delPD(IAID);
}

void TClntAddrMgr::addPD(SmartPtr<TAddrIA> ptr) {
    Client->addPD(ptr);
}

int TClntAddrMgr::countPD() {
    return Client->countPD();
}

bool TClntAddrMgr::isPDAssigned(unsigned long IAID)
{
	SmartPtr<TAddrIA> pd;
	Client->firstPD();
	while (pd=Client->getPD())
		if (pd->getIAID()==IAID) return true;
	return false;
}	

SmartPtr<TAddrIA> TClntAddrMgr::getPD(unsigned long IAID)
{
    SmartPtr<TAddrIA> ptrPD;
    this->Client->firstPD();
    while (ptrPD = this->Client->getPD() ) {
	if (ptrPD->getIAID() == IAID)
	    return ptrPD;
    }
    return 0;
}




void TClntAddrMgr::firstTA() 
{
    Client->firstTA();
}

SmartPtr<TAddrIA> TClntAddrMgr::getTA()
{
    return Client->getTA();
}

SmartPtr<TAddrIA> TClntAddrMgr::getTA(unsigned long iaid)
{
    SmartPtr<TAddrIA> ta;
    this->Client->firstTA();
    while (ta = this->Client->getTA() ) {
	if (ta->getIAID() == iaid)
	    return ta;
    }
    return 0;
}

void TClntAddrMgr::addTA(SmartPtr<TAddrIA> ptr)
{
    Client->addTA(ptr);
}

bool TClntAddrMgr::delTA(unsigned long iaid)
{
    return Client->delTA(iaid);
}

int TClntAddrMgr::countTA()
{
    return Client->countTA();
}

void TClntAddrMgr::print(ostream &x) {
    
}

bool TClntAddrMgr::addPrefix(SmartPtr<TDUID> srvDuid , SmartPtr<TIPv6Addr> srvAddr,
			     int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
			     SmartPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			     int length, bool quiet) 
{
    if (!prefix) {
	Log(Error) << "Attempt to add null prefix failed." << LogEnd;
	return false;
    }

    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    ptrClient = getClient();

    return TAddrMgr::addPrefix(ptrClient, srvDuid, srvAddr, iface, IAID, T1, T2, prefix, 
			       pref, valid, length, quiet);
}

bool TClntAddrMgr::updatePrefix(SPtr<TDUID> srvDuid , SPtr<TIPv6Addr> srvAddr,
				int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
				SmartPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
				int length, bool quiet)
{
    if (!prefix) {
	Log(Error) << "Attempt to update null prefix failed." << LogEnd;
	return false;
    }

    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    ptrClient = getClient();

    return TAddrMgr::updatePrefix(ptrClient, srvDuid, srvAddr, iface, IAID, T1, T2, prefix, 
				  pref, valid, length, quiet);
}
