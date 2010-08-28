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
#include "AddrClient.h"
#include "Logger.h"

TClntAddrMgr * TClntAddrMgr::Instance = 0;

void TClntAddrMgr::instanceCreate(SPtr<TDUID> clientDUID, bool useConfirm, string xmlFile, bool loadDB)
{
  if (Instance) {
      Log(Crit) << "Attempt to create another instance of TClntAddrMgr!" << LogEnd;
      return;
  }
  Instance = new TClntAddrMgr(clientDUID, useConfirm, xmlFile, loadDB);
}

TClntAddrMgr& TClntAddrMgr::instance()
{
  return *Instance;
}

/**
 * @brief Client Address Manager constructor
 *
 * constructor used for creation of client version of address manager
 *
 * @param clientDUID pointer to client DUID
 * @param useConfirm should confirm be used or not?
 * @param xmlFile XML filename (AddrMgr dumps will be stored there)
 * @param loadDB should existing dump be loaded?
 *
 */
TClntAddrMgr::TClntAddrMgr(SPtr<TDUID> clientDUID, bool useConfirm, string xmlFile, bool loadDB)
    :TAddrMgr(xmlFile, useConfirm)
{
    // client may have been already loaded from client-AddrMgr.xml file
    firstClient();
    if (!getClient()) {
        // add this client (with proper duid)
        SPtr<TAddrClient> client = new TAddrClient(clientDUID);
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
    SPtr<TAddrIA> ptrIA;
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

/**
 * @brief Removes outdated addresses
 *
 * removes addresses that already expired. May also perform
 * other duties. This method should be called periodically.
 *
 */
void TClntAddrMgr::doDuties()
{
    SPtr<TAddrIA> ptrIA;
    SPtr<TAddrAddr> ptrAddr;

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

SPtr<TAddrIA> TClntAddrMgr::getIA() {
    return Client->getIA();
}

bool TClntAddrMgr::delIA(long IAID) {
    return Client->delIA(IAID);
}

void TClntAddrMgr::addIA(SPtr<TAddrIA> ptr) {
    Client->addIA(ptr);
}

int TClntAddrMgr::countIA() {
    return Client->countIA();
}

TClntAddrMgr::~TClntAddrMgr() {
    this->Client = 0;
    Log(Debug) << "ClntAddrMgr cleanup." << LogEnd;
}

SPtr<TAddrIA> TClntAddrMgr::getIA(unsigned long IAID)
{
    SPtr<TAddrIA> ptrIA;
    this->Client->firstIA();
    while (ptrIA = this->Client->getIA() ) {
	if (ptrIA->getIAID() == IAID)
	    return ptrIA;
    }
    return 0;
}

/** 
 * sets specified interface to CONFIRMME state
 * when network switch off signal received, the funtion will be invoked to 
 * set valid IA to CONFIRMME state.
 * 
 * @param changedLinks structure containing interface indexes to be confirmed
 */
void TClntAddrMgr::setIA2Confirm(volatile link_state_notify_t * changedLinks)
{
    SPtr<TAddrIA> ptrIA;
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

SPtr<TAddrIA> TClntAddrMgr::getPD() {
    return Client->getPD();
}

bool TClntAddrMgr::delPD(long IAID) {
    return Client->delPD(IAID);
}

void TClntAddrMgr::addPD(SPtr<TAddrIA> ptr) {
    Client->addPD(ptr);
}

int TClntAddrMgr::countPD() {
    return Client->countPD();
}

SPtr<TAddrIA> TClntAddrMgr::getPD(unsigned long IAID)
{
    SPtr<TAddrIA> ptrPD;
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

SPtr<TAddrIA> TClntAddrMgr::getTA()
{
    return Client->getTA();
}

SPtr<TAddrIA> TClntAddrMgr::getTA(unsigned long iaid)
{
    SPtr<TAddrIA> ta;
    this->Client->firstTA();
    while (ta = this->Client->getTA() ) {
	if (ta->getIAID() == iaid)
	    return ta;
    }
    return 0;
}

void TClntAddrMgr::addTA(SPtr<TAddrIA> ptr)
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

/**
 * @brief Adds a prefix. 
 *
 * @param srvDuid Server DUID 
 * @param srvAddr Server address. 
 * @param iface   interface index
 * @param IAID    IAID
 * @param T1      T1 timer
 * @param T2      T2 timer
 * @param prefix  prefix
 * @param pref    prefered lifetime
 * @param valid   valid lifetime
 * @param length  prefix length
 * @param quiet   quiet mode (true=be quiet)
 *
 * @return true if successful, false otherwise
 */
bool TClntAddrMgr::addPrefix(SPtr<TDUID> srvDuid , SPtr<TIPv6Addr> srvAddr,
			     int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
			     SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			     int length, bool quiet) 
{
    if (!prefix) {
	Log(Error) << "Attempt to add null prefix failed." << LogEnd;
	return false;
    }

    // find this client
    SPtr <TAddrClient> ptrClient;
    this->firstClient();
    ptrClient = getClient();

    return TAddrMgr::addPrefix(ptrClient, srvDuid, srvAddr, iface, IAID, T1, T2, prefix, 
			       pref, valid, length, quiet);
}

bool TClntAddrMgr::updatePrefix(SPtr<TDUID> srvDuid , SPtr<TIPv6Addr> srvAddr,
				int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
				SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
				int length, bool quiet)
{
    if (!prefix) {
	Log(Error) << "Attempt to update null prefix failed." << LogEnd;
	return false;
    }

    // find this client
    SPtr <TAddrClient> ptrClient;
    this->firstClient();
    ptrClient = getClient();

    return TAddrMgr::updatePrefix(ptrClient, srvDuid, srvAddr, iface, IAID, T1, T2, prefix, 
				  pref, valid, length, quiet);
}

SPtr<TIPv6Addr> TClntAddrMgr::getPreferredAddr() {
    SPtr<TAddrIA> ia;
    SPtr<TAddrAddr> addr;
    
    firstIA();
    while ( ia = getIA() ) {
	if (ia->getTentative() != TENTATIVE_NO)
	    continue;

	ia->firstAddr();
	while (addr = ia->getAddr()) {
	    if (addr->getTentative() == TENTATIVE_NO)
		return addr->get(); // return the first address from first non-tentative
	    //if (is_addr_tentative(NULL, ia->getIface(), addr->get()->getPlain()) == LOWLEVEL_TENTATIVE_NO)
	}
    }

    return 0;
}
