#include "SrvAddrMgr.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include "AddrAddr.h"
#include "Logger.h"
#include "SrvCfgAddrClass.h"

TSrvAddrMgr::TSrvAddrMgr(string addrdb, bool loadDB) : TAddrMgr(addrdb,loadDB)
{
    
}

long TSrvAddrMgr::getTimeout()
{
    return this->getValidTimeout();
}

bool TSrvAddrMgr::addClntAddr(SmartPtr<TDUID> duid ,
			      long IAID, SmartPtr<TIPv6Addr> addr, long pref, long valid)
{
    SmartPtr <TAddrClient> ptrClient;
    ClntsLst.first();
    while ( ptrClient = ClntsLst.get() ) {
        if ( (*ptrClient->getDUID()) == (*duid) ) 
            break;
    }

    // Have we found this client? 
    if (!ptrClient) {
        std::clog << logger::logWarning << "Client not found in addrDB, can't add Addr" << logger::endl;
        return false;
    }

    SmartPtr <TAddrIA> ptrIA;
    ptrClient->firstIA();

    while ( ptrIA = ptrClient->getIA() ) {
        if ( ptrIA->getIAID() == IAID)
            break;
    }

    if (!ptrIA) {
        std::clog << logger::logWarning << "IA (IAID=" << IAID << ") not found." << logger::endl;
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
        std::clog << logger::logWarning << "Addr already exists." << logger::endl;
        return false;
    }

    // add address
    ptrAddr = new TAddrAddr(addr, pref, valid);
    ptrIA->addAddr(ptrAddr);
    return true;
}

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
        std::clog << logger::logNotice << "Client not found in addrDB, so getAddrCount()=0." << logger::endl;
        return false;
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
    std::clog << logger::logDebug << "getAddrsCount() results: DUID=" << *duid;
    for (int i=0; i< classNr; i++) {
	std::clog << "addrCount=[" << *(addrCnt+i) << "] clntCount=[" << *(clntCnt+i) << "]" << logger::endl;
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
                            std::clog << logger::logNotice << "Addr " << *(ptrAddr->get()) << " in IA (IAID="
                                << ptrIA->getIAID() << ") in client (DUID=\"";
                            if (ptrClient->getDUID()) 
                            {
                                std::clog<<*ptrClient->getDUID();
                                /*for (int i=0; i<ptrClient->getDUIDlen(); i++) 
                                {
                                    std::clog.fill('0');
                                    std::clog << hex << (unsigned short)(DUID[i]);
                                } */
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
