/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvAddrMgr.cpp,v 1.9 2004-12-07 00:45:10 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2004/12/03 20:51:42  thomson
 * Logging issues fixed.
 *
 * Revision 1.7  2004/09/07 22:02:33  thomson
 * pref/valid/IAID is not unsigned, RAPID-COMMIT now works ok.
 *
 * Revision 1.6  2004/06/21 23:08:48  thomson
 * Minor fixes.
 *
 * Revision 1.5  2004/06/20 21:00:26  thomson
 * quiet flag added.
 *
 * Revision 1.4  2004/06/20 19:29:23  thomson
 * New address assignment finally works.
 *
 * Revision 1.3  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 *                                                                           
 */

#include "SrvAddrMgr.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include "SrvCfgAddrClass.h"

TSrvAddrMgr::TSrvAddrMgr(string xmlfile) 
    :TAddrMgr(xmlfile, false) {
    
}

TSrvAddrMgr::~TSrvAddrMgr() {
    Log(Debug) << "SrvAddrMgr cleanup." << LogEnd;
}

long TSrvAddrMgr::getTimeout() {
    return this->getValidTimeout();
}


/*
 * add address for a client. If client o IA is missing, add it, too.
 */
bool TSrvAddrMgr::addClntAddr(SmartPtr<TDUID> clntDuid , SmartPtr<TIPv6Addr> clntAddr,
			      int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
			      SmartPtr<TIPv6Addr> addr, unsigned long pref, unsigned long valid,
			      bool quiet) {
    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) ) 
            break;
    }

    // have we found this client? 
    if (!ptrClient) {
	if (!quiet) Log(Debug) << "Adding client (DUID=" << clntDuid->getPlain()
			       << ") to addrDB." << LogEnd;
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
	if (!quiet)
	    Log(Debug) << "Adding IA (IAID=" << IAID << ") to addrDB." << LogEnd;
    }

    SmartPtr <TAddrAddr> ptrAddr;
    ptrIA->firstAddr();
    while ( ptrAddr = ptrIA->getAddr() ) {
        if (*ptrAddr->get()==*addr)
            break;
    }

    // address already exists
    if (ptrAddr) {
        Log(Warning) << "Address " << *ptrAddr 
		             << " is already assigned to this IA." << LogEnd;
        return false;
    }

    // add address
    ptrAddr = new TAddrAddr(addr, pref, valid);
    ptrIA->addAddr(ptrAddr);
    if (!quiet)
	Log(Debug) << "Adding " << ptrAddr->get()->getPlain() 
		   << " to IA (IAID=" << IAID 
		   << ") to addrDB." << LogEnd;
    return true;
}

/*
 *  Frees address (also deletes IA and/or client, if this was last address)
 */
bool TSrvAddrMgr::delClntAddr(SmartPtr<TDUID> clntDuid,
			      unsigned long IAID, SmartPtr<TIPv6Addr> clntAddr,
			      bool quiet) {
    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) ) 
            break;
    }

    // have we found this client? 
    if (!ptrClient) {
        Log(Warning) << "Client (DUID=" << clntDuid->getPlain() 
		     << ") not found in addrDB, cannot delete address and/or client." << LogEnd;
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
	Log(Warning) << "Address " << *clntAddr << " not assigned, cannot delete." << LogEnd;
	return false;
    }

    ptrIA->delAddr(clntAddr);
    if (!quiet)
	Log(Debug) << "Deleted address " << *clntAddr << " from addrDB." << LogEnd;
    
    if (!ptrIA->countAddr()) {
	if (!quiet)
	    Log(Debug) << "Deleted IA (IAID=" << IAID << ") from addrDB." << LogEnd;
	ptrClient->delIA(IAID);
    }

    if (!ptrClient->countIA()) {
	if (!quiet)
	    Log(Debug) << "Deleted client (DUID=" << clntDuid->getPlain()
		      << ") form addrDB." << LogEnd;
	this->delClient(clntDuid);
    }

    return true;
}


/*
 * how many addresses does this client have?
 */
unsigned long TSrvAddrMgr::getAddrCount(SmartPtr<TDUID> duid)
{
    SmartPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() ) {
        if ( (*ptrClient->getDUID()) == (*duid))
            break;
    }
    // Have we found this client? 
    if (!ptrClient) {
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
                                Log(Cont) << *ptrClient->getDUID();
                            }
                            Log(Cont) << "\") has expired." << dec << LogEnd;
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
        this->dump();
}
