/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntAddrMgr.cpp,v 1.11.2.1 2006-02-05 23:38:06 thomson Exp $
 */

#include "SmartPtr.h"
#include "AddrIA.h"
#include "ClntAddrMgr.h"
#include "ClntCfgMgr.h"
#include "AddrClient.h"
#include "Logger.h"


TClntAddrMgr::TClntAddrMgr(SmartPtr<TClntCfgMgr> ClntCfgMgr, string xmlFile, bool loadDB)
    :TAddrMgr(xmlFile, loadDB)
{
    
    // add this client (with proper duid)
    SmartPtr<TAddrClient> client = new TAddrClient(ClntCfgMgr->getDUID());
    addClient(client);

    // set Client field
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
		Log(Warning) << "Address " << ptrAddr->get()->getPlain() 
			     << " from IA " << ptrIA->getIAID() 
			     << " has been removed from addrDB." << LogEnd;
		ptrIA->setState(NOTCONFIGURED);
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
    while (ta = this->Client->getIA() ) {
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


/*
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/08/03 23:17:11  thomson
 * Minor changes fixed.
 *
 * Revision 1.10  2004/12/08 00:15:07  thomson
 * Fixed issues with denied RENEW (bug #53)
 *
 * Revision 1.9  2004/12/07 22:54:35  thomson
 * Typos corrected.
 *
 * Revision 1.8  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.7  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.6  2004/10/27 22:07:55  thomson
 * Signed/unsigned issues fixed, Lifetime option implemented, INFORMATION-REQUEST
 * message is now sent properly. Valid lifetime granted by server fixed.
 *
 * Revision 1.5  2004/09/08 21:22:45  thomson
 * Parser improvements, signed/unsigned issues addressed.
 *
 * Revision 1.4  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 */
