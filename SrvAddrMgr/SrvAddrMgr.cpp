/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvAddrMgr.cpp,v 1.3 2004-06-17 23:53:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#include "SrvAddrMgr.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include "SrvCfgAddrClass.h"

TSrvAddrMgr::TSrvAddrMgr(string addrdb, bool loadDB) 
    :TAddrMgr(addrdb,loadDB) {
    
}

long TSrvAddrMgr::getTimeout() {
    return this->getValidTimeout();
}


/*
 * add address for a client. If client o IA is missing, add it, too.
 */
bool TSrvAddrMgr::addClntAddr(SmartPtr<TDUID> clntDuid , SmartPtr<TIPv6Addr> clntAddr,
			      int iface, long IAID, unsigned long T1, unsigned long T2, 
			      SmartPtr<TIPv6Addr> addr, long pref, long valid) {
    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) ) 
            break;
    }

    // have we found this client? 
    if (!ptrClient) {
        Log(Notice) << "Client not found in addrDB, adding it." << LogEnd;
	ptrClient = new TAddrClient(clntDuid);
	this->addClient(ptrClient);
    }

    // find this IA
    SmartPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();
    while ( ptrIA = ptrClient->getIA() ) {
        if ( ptrIA->getIAID() == IAID)
            break;
    }

    // have we found this IA?
    if (!ptrIA) {
	ptrIA = new TAddrIA(iface, clntAddr, clntDuid, T1, T2, IAID);
	ptrClient->addIA(ptrIA);
        Log(Info) << "IA (IAID=" << IAID << ") not found in addrDB, adding it." << LogEnd;
        return false;
    }

    SmartPtr <TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        if (*ptrAddr->get()==*addr)//( !memcmp(ptrAddr->get(), addr, 16))
            break;
    }
    // address already exists
    if (ptrAddr) {
        Log(Warning) << "Address " << *ptrAddr << " is already assigned in addrDB." << logger::endl;
        return false;
    }

    // add address
    ptrAddr = new TAddrAddr(addr, pref, valid);
    ptrIA->addAddr(ptrAddr);
    return true;
}

/*
 *  Frees address (also deletes IA and/or client, if this was last address)
 */
bool TSrvAddrMgr::delClntAddr(SmartPtr<TDUID> clntDuid ,
			      long IAID, SmartPtr<TIPv6Addr> clntAddr) {
    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) ) 
            break;
    }

    // have we found this client? 
    if (!ptrClient) {
        Log(Warning) << "Client not found in addrDB, cannot delete address and/or client." << LogEnd;
	return false;
    }

    // find this IA
    SmartPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();
    while ( ptrIA = ptrClient->getIA() ) {
        if ( ptrIA->getIAID() == IAID)
            break;
    }

    // have we found this IA?
    if (!ptrIA) {
        Log(Warning) << "IA (IAID=" << IAID << ") not assigned to client, cannot delete address and/or IA."
		     << LogEnd;
        return false;
    }

    SmartPtr <TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        if (*ptrAddr->get()==*clntAddr)
            break;
    }

    // address already exists
    if (!ptrAddr) {
	Log(Warning) << "Address " << *ptrAddr << " not assigned, cannot delete." << LogEnd;
    }

    ptrIA->delAddr(clntAddr);
    Log(Info) << "Address " << clntAddr << " deleted from addrDB." << LogEnd;
    
    if (!ptrIA->countAddr()) {
	Log(Info) << "IA (IAID=" << IAID << ") no longer used, removing." << LogEnd;
	ptrClient->delIA(IAID);
    }

    if (!ptrClient->countIA()) {
	Log(Info) << "Client no longer has any IAs or addresses, removing this client." << LogEnd;
	this->delClient(clntDuid);
    }

    return true;
}


/*
 * how many addresses does this client have?
 */
unsigned long TSrvAddrMgr::getAddrCount(SmartPtr<TDUID> duid, int iface)
{
    SmartPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() ) {
        if ( (*ptrClient->getDUID()) == (*duid))
            break;
    }
    // Have we found this client? 
    if (!ptrClient) {
	Log(Info) << "Client is not present in addrDB." << LogEnd;
        return 0;
    }

    unsigned long count=0;
    
    // look at each of client's IAs
    SmartPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();
    while ( ptrIA = ptrClient->getIA() ) {
        count += ptrIA->countAddr();
    }
    return count;
}

bool TSrvAddrMgr::addrIsFree(SmartPtr<TIPv6Addr> addr)
{
    // for each client...
    SmartPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() ) 
    {

        // look at each client's IAs
        SmartPtr <TAddrIA> ptrIA;
        ptrClient->firstIA();
        while ( ptrIA = ptrClient->getIA() ) 
        {
            SmartPtr<TAddrAddr> ptrAddr;
            if (ptrAddr = ptrIA->getAddr(addr) )
                return false;
        }
    }
    return true;
}

void TSrvAddrMgr::getAddrsCount(
     SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > classes,
     long    *clntCnt,
     long    *addrCnt,
     SmartPtr<TDUID> duid, 
     int iface)
{   
    memset(clntCnt,0,sizeof(long)*classes->count());
    memset(addrCnt,0,sizeof(long)*classes->count());
    int classNr=0;

    SmartPtr<TAddrClient> ptrClient;
    firstClient();
    while(ptrClient=getClient())
    {
        bool thisClient=(*(ptrClient->getDUID())==*duid);
        ptrClient->firstIA();
        SmartPtr<TAddrIA> ptrIA;
        while(ptrIA=ptrClient->getIA())
        {
            SmartPtr<TAddrAddr> ptrAddr;
            ptrIA->firstAddr();
            while(ptrAddr=ptrIA->getAddr())
            {
                if(ptrIA->getIface()==iface)
                {
                    SmartPtr<TSrvCfgAddrClass> ptrClass;
                    classes->first();
		    classNr=0;
                    while(ptrClass=classes->get())
                    {
                        if(ptrClass->addrInPool(ptrAddr->get()))
                        {
                            if(thisClient)
                                clntCnt[classNr]++;
                            addrCnt[classNr]++;
                        }
                        classNr++;
                    }
                }
            }
        }            
    }
}

// remove outdated addresses 
void TSrvAddrMgr::doDuties()
{
    SmartPtr<TAddrClient> ptrClient;
    SmartPtr<TAddrIA>     ptrIA;
    SmartPtr<TAddrAddr>   ptrAddr;
    bool anyDeleted=false;
    // for each client...
    this->firstClient();
    while (ptrClient = this->getClient() ) 
    {
        // ... which has outdated adresses
        if (!ptrClient->getValidTimeout()) 
        {
            ptrClient->firstIA();
            while ( ptrIA = ptrClient->getIA() ) 
            {
                // has this IA outdated addressess?
                if ( !ptrIA->getValidTimeout() ) 
                {
                    ptrIA->firstAddr();
                    while ( ptrAddr = ptrIA->getAddr() )  
                    {
                        if (!ptrAddr->getValidTimeout()) 
                        {
                            // delete this address
			    Log(Notice) << "Addr " << *(ptrAddr->get()) << " in IA (IAID="
					<< ptrIA->getIAID() << ") in client (DUID=\"";
                            if (ptrClient->getDUID()) 
                            {
                                std::clog << *ptrClient->getDUID();
                            }
                            std::clog << "\") has expired." << dec << logger::endl;
                            ptrIA->delAddr(ptrAddr->get());
                            anyDeleted=true;
                        }
                    }
                    if (!ptrIA->getAddrCount())
                    {
                        anyDeleted=true;
                        ptrClient->delIA(ptrIA->getIAID());
                    }
                }
            }
        }
        if (!(ptrClient->countIA()))
        {
            this->delClient(ptrClient->getDUID());
            anyDeleted=true;
        };
    }
    if (anyDeleted) 
        this->dbStore();
}
