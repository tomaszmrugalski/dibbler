/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Micha³ Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: AddrMgr.cpp,v 1.31 2008-02-25 17:49:06 thomson Exp $
 *
 */

#include <iostream>
#include <fstream>
#include "Portable.h"
#include <limits.h>
#include "AddrMgr.h"
#include "AddrClient.h"
#include "Logger.h"

TAddrMgr::TAddrMgr(string xmlFile, bool loadfile)
{
    this->IsDone = false;
    this->XmlFile = xmlFile;
    
    if (loadfile)
	dbLoad();

    DeleteEmptyClient = true;
}

void TAddrMgr::dbLoad()
{

#ifdef LIBXML2
     xmlDocPtr root;
     root = xmlLoad(this->dbfile.c_str());
     if (!root) {
 	Log(Error) << "File loading has failed." << LogEnd;
     }
     this->parseAddrMgr(root,0);
     xmlFreeDoc(root);
#endif
}

void TAddrMgr::dump()
{
    std::ofstream xmlDump;
    xmlDump.open(this->XmlFile.c_str(), ios::ate);
    xmlDump << *this;
    xmlDump.close();
}

void TAddrMgr::addClient(SmartPtr<TAddrClient> x)
{
    ClntsLst.append(x);
}

void TAddrMgr::firstClient()
{
    ClntsLst.first();
}

SmartPtr<TAddrClient> TAddrMgr::getClient()
{
    return ClntsLst.get();
}

SPtr<TAddrClient> TAddrMgr::getClient(SmartPtr<TDUID> duid)
{
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();
    while (ptr = ClntsLst.get() ) 
    {
        if ( *(ptr->getDUID()) == (*duid) )
            return ptr;
    }
    return SmartPtr<TAddrClient>();
}

SmartPtr<TAddrClient> TAddrMgr::getClient(uint32_t SPI)
{
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();
    while (ptr = ClntsLst.get() ) 
    {
        if ( ptr->getSPI() == SPI )
            return ptr;
    }
    return SmartPtr<TAddrClient>();
}

SPtr<TAddrClient> TAddrMgr::getClient(SPtr<TIPv6Addr> leasedAddr)
{
    SPtr<TAddrClient> cli;
    ClntsLst.first();
    while (cli = ClntsLst.get() ) 
    {
	SPtr<TAddrIA> ia;
	cli->firstIA();
	while (ia = cli->getIA()) {
	    if ( ia->getAddr(leasedAddr) )
		return cli;
	}
    }

    return 0;
}

int TAddrMgr::countClient()
{
    return ClntsLst.count();
}

bool TAddrMgr::delClient(SmartPtr<TDUID> duid)
{
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();

    while ( ptr = ClntsLst.get() ) 
    {
        if  ((*ptr->getDUID())==(*duid)) 
        {
            ClntsLst.del();
            return true;
        }
    }
    return false;
}

// --------------------------------------------------------------------
// --- time related methods -------------------------------------------
// --------------------------------------------------------------------

unsigned long TAddrMgr::getT1Timeout()
{
    unsigned long ts = ULONG_MAX;
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();
    while (ptr = ClntsLst.get() ) {
        if (ts > ptr->getT1Timeout() )
            ts = ptr->getT1Timeout();
    }
    return ts;
}

unsigned long TAddrMgr::getT2Timeout()
{
    unsigned long ts = ULONG_MAX;
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();
    while (ptr = ClntsLst.get() ) {
        if (ts > ptr->getT2Timeout() )
            ts = ptr->getT2Timeout();
    }
    return ts;
}

unsigned long TAddrMgr::getPrefTimeout()
{
    unsigned long ts = ULONG_MAX;
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();
    while (ptr = ClntsLst.get() ) {
        if (ts > ptr->getPrefTimeout() )
            ts = ptr->getPrefTimeout();
    }
    return ts;
}

unsigned long TAddrMgr::getValidTimeout()
{
    unsigned long ts = ULONG_MAX;
    SmartPtr<TAddrClient> ptr;
    ClntsLst.first();
    while (ptr = ClntsLst.get() ) {
        if (ts > ptr->getValidTimeout() )
            ts = ptr->getValidTimeout();
    }
    return ts;
}

unsigned long TAddrMgr::getAddrCount(SmartPtr<TDUID> duid, int iface)
{
    // Not used on the client side, server side uses SrvAddrMgr::getAddrCount()
    return 1;
}

/* Prefix Delegation-related method starts here */

/** 
 * addx prefix for a client. If client's IA is missing, add it, too.
 * 
 * @param clntDuid 
 * @param clntAddr 
 * @param iface 
 * @param IAID 
 * @param T1 
 * @param T2 
 * @param addr 
 * @param pref 
 * @param valid 
 * @param quiet 
 * 
 * @return 
 */
bool TAddrMgr::addPrefix(SPtr<TDUID> clntDuid , SPtr<TIPv6Addr> clntAddr,
                         int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
                         SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
                         int length, bool quiet) {
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
    return addPrefix(ptrClient, clntDuid, clntAddr, iface, IAID, T1, T2, prefix, pref, valid, length, quiet);
}

bool TAddrMgr::addPrefix(SPtr<TAddrClient> client, SmartPtr<TDUID> duid , SmartPtr<TIPv6Addr> addr,
			 int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
			 SmartPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			 int length, bool quiet) {
    if (!prefix) {
	Log(Error) << "Attempt to add null prefix failed." << LogEnd;
	return false;
    }

    if (!client) {
	Log(Error) << "Unable to add prefix, client not defined." << LogEnd;
	return false;
    }

    // find this PD
    SmartPtr <TAddrIA> ptrPD;
    client->firstPD();
    while ( ptrPD = client->getPD() ) {
        if ( ptrPD->getIAID() == IAID)
            break;
    }

    // have we found this PD?
    if (!ptrPD) {
	ptrPD = new TAddrIA(iface, addr, duid, T1, T2, IAID);
	client->addPD(ptrPD);
	if (!quiet)
	    Log(Debug) << "PD: Adding PD (iaid=" << IAID << ") to addrDB." << LogEnd;
    }

    SmartPtr <TAddrPrefix> ptrPrefix;
    ptrPD->firstPrefix();
    while ( ptrPrefix = ptrPD->getPrefix() ) {
        if (*ptrPrefix->get()==*prefix)
            break;
    }

    // address already exists
    if (ptrPrefix) {
        Log(Warning) << "PD: Prefix " << *ptrPrefix
		     << " is already assigned to this PD." << LogEnd;
        return false;
    }

    // add address
    ptrPD->addPrefix(prefix, pref, valid, length);
    if (!quiet)
	Log(Debug) << "PD: Adding " << prefix->getPlain() 
		   << " prefix to PD (iaid=" << IAID 
		   << ") to addrDB." << LogEnd;
    return true;
}

bool TAddrMgr::updatePrefix(SPtr<TDUID> duid , SPtr<TIPv6Addr> addr,
			    int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
			    SmartPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			    int length, bool quiet)
{
    // find client...
    SmartPtr <TAddrClient> client;
    this->firstClient();
    while ( client = this->getClient() ) {
        if ( (*client->getDUID()) == (*duid) ) 
            break;
    }
    if (!client) {
	Log(Error) << "Unable to update prefix " << prefix->getPlain() << "/" << (int)length << ": DUID=" << duid->getPlain() << " not found." << LogEnd;
	return false;
    }

    return updatePrefix(client, duid, addr, iface, IAID, T1, T2, prefix, pref, valid, length, quiet);
}

bool TAddrMgr::updatePrefix(SPtr<TAddrClient> client, SmartPtr<TDUID> duid , SmartPtr<TIPv6Addr> clntAddr,
			    int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
			    SmartPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			    int length, bool quiet)
{
    if (!prefix) {
	Log(Error) << "Attempt to update null prefix failed." << LogEnd;
	return false;
    }
    if (!client) {
	Log(Error) << "Unable to update prefix, client not defined." << LogEnd;
	return false;
    }
    
    // for that client, find IA
    SmartPtr <TAddrIA> pd;
    client->firstPD();
    while ( pd = client->getPD() ) {
        if ( pd->getIAID() == IAID)
            break;
    }
    // have we found this PD?
    if (!pd) {
	Log(Error) << "Unable to find PD (iaid=" << IAID << ") for client " << duid->getPlain() << "." << LogEnd;
	return false;
    }
    pd->setTimestamp();
    pd->setT1(T1);
    pd->setT2(T2);

    SmartPtr <TAddrPrefix> ptrPrefix;
    pd->firstPrefix();
    while ( ptrPrefix = pd->getPrefix() ) {
        if (*ptrPrefix->get()==*prefix)
            break;
    }

    // address already exists
    if (!ptrPrefix) {
        Log(Warning) << "PD: Prefix " << prefix->getPlain() << " is not known. Unable to update." << LogEnd;
        return false;
    }

    ptrPrefix->setTimestamp();
    ptrPrefix->setPref(pref);
    ptrPrefix->setValid(pref);
    
    return true;
}

/*
 *  Frees prefix (also deletes IA and/or client, if this was last address)
 */
bool TAddrMgr::delPrefix(SmartPtr<TDUID> clntDuid,
			    unsigned long IAID, SmartPtr<TIPv6Addr> prefix,
			    bool quiet) {

    Log(Debug) << "PD: Deleting prefix " << prefix->getPlain() << ", DUID=" << clntDuid->getPlain() << ", iaid=" << IAID << LogEnd;
    // find this client
    SmartPtr <TAddrClient> ptrClient;
    this->firstClient();
    while ( ptrClient = this->getClient() ) {
        if ( (*ptrClient->getDUID()) == (*clntDuid) ) 
            break;
    }

    // have we found this client? 
    if (!ptrClient) {
        Log(Warning) << "PD: Client (DUID=" << clntDuid->getPlain() 
		     << ") not found in addrDB, cannot delete address and/or client." << LogEnd;
	return false;
    }

    // find this IA
    SmartPtr <TAddrIA> ptrPD;
    ptrClient->firstPD();
    while ( ptrPD = ptrClient->getPD() ) {
        if ( ptrPD->getIAID() == IAID)
            break;
    }

    // have we found this IA?
    if (!ptrPD) {
        Log(Warning) << "PD: iaid=" << IAID << " not assigned to client, cannot delete address and/or PD."
		     << LogEnd;
        return false;
    }

    SmartPtr <TAddrPrefix> ptrPrefix;
    ptrPD->firstPrefix();
    while ( ptrPrefix = ptrPD->getPrefix() ) {
        if (*ptrPrefix->get()==*prefix)
            break;
    }

    // address already exists
    if (!ptrPrefix) {
	Log(Warning) << "PD: Prefix " << *prefix << " not assigned, cannot delete." << LogEnd;
	return false;
    }

    ptrPD->delPrefix(prefix);

    // FIXME: Cache for prefixes this->addCachedAddr(clntDuid, clntAddr);
    if (!quiet)
	Log(Debug) << "PD: Deleted prefix " << *prefix << " from addrDB." << LogEnd;
    
    if (!ptrPD->getPrefixCount()) {
	if (!quiet)
	    Log(Debug) << "PD: Deleted PD (iaid=" << IAID << ") from addrDB." << LogEnd;
	ptrClient->delPD(IAID);
    }

    if (!ptrClient->countIA() && !ptrClient->countTA() && !ptrClient->countPD() && DeleteEmptyClient) {
      if (!quiet)
	    Log(Debug) << "PD: Deleted client (DUID=" << clntDuid->getPlain()
                   << ") from addrDB." << LogEnd;
      this->delClient(clntDuid);
    }

    return true;
}


/** 
 * checks if a specific prefix is used
 * 
 * @param x 
 * 
 * @return true - used, false - free
 */
bool TAddrMgr::prefixIsFree(SPtr<TIPv6Addr> x)
{
    SPtr<TAddrClient> client;
    SPtr<TAddrIA> pd;
    SPtr<TAddrPrefix> prefix;

    firstClient();
    while (client = getClient()) {
	client->firstPD();
	while (pd = client->getPD()) {
	    pd->firstPrefix();
	    while (prefix = pd->getPrefix()) {
		if (*prefix->get()==*x)
		    return false;
	    }
	}
    }

    /* prefix not found, so it's free */
    return true;
}

/* PD-ends */

// --------------------------------------------------------------------
// --- XML-related methods --------------------------------------------
// --------------------------------------------------------------------
#ifdef LIBXML2
// loads entire file
xmlDocPtr TAddrMgr::xmlLoad(const char * filename) {
     xmlDocPtr doc;
     xmlNodePtr cur;

     doc = xmlParseFile(filename);
	
     if (doc == NULL ) {
 	fprintf(stderr,"Document not parsed successfully. \n");
 	return NULL;
     }

     cur = xmlDocGetRootElement(doc);
	
     if (cur == NULL) {
 	fprintf(stderr,"empty document\n");
 	xmlFreeDoc(doc);
 	return NULL;
     }

     if (xmlStrcmp(cur->name, (const xmlChar *) "AddrMgr")) {
 	fprintf(stderr,"document of the wrong type, root node != AddrMgr\n");
 	xmlFreeDoc(doc);
 	return NULL;
     }

     // DTD
     xmlDtdPtr dtd = xmlParseDTD(NULL, ADDRDB_DTD); /* parse the DTD */
     if (!dtd)
     {
 	Log(Error) << "DTD load failed." << LogEnd;
 	return NULL;
     }

     return doc;
  //  return NULL;
}

SmartPtr<TAddrAddr> TAddrMgr::parseAddrAddr(xmlDocPtr doc, xmlNodePtr xmlAddr, int depth)
{
     // timestamp
     xmlChar * tsStr = xmlGetProp(xmlAddr,(const xmlChar*)"timestamp");
     unsigned long ts = atol((const char *)tsStr);

     // prefered-lifetime
     xmlChar * prefStr = xmlGetProp(xmlAddr,(const xmlChar*)"pref");
     unsigned long pref = atol((const char *)prefStr);

     // valid-lifetime
     xmlChar * validStr = xmlGetProp(xmlAddr,(const xmlChar*)"valid");
     unsigned long valid = atol((const char *)validStr);

     // address
     xmlChar * addr;
     addr = xmlNodeListGetString(doc, xmlAddr->xmlChildrenNode, 1);
     char addrPacked[16];

     inet_pton6((const char*)addr,addrPacked);

     SmartPtr<TAddrAddr> ptrAddr = new TAddrAddr(new TIPv6Addr(addrPacked),pref,valid);
     ptrAddr->setTimestamp(ts);

     xmlFree(addr);
     xmlFree(tsStr);
     xmlFree(prefStr);
     xmlFree(validStr);

     return ptrAddr;
//   return NULL;
}

SmartPtr<TAddrIA> TAddrMgr::parseAddrIA(xmlDocPtr doc, xmlNodePtr xmlIA, int depth)
{
     // DUID
     xmlChar * DUIDStr = xmlGetProp(xmlIA,(const xmlChar*)"DUID");
     SmartPtr <TDUID> ptrDUID = new TDUID((char*)DUIDStr);

     // unicast
     xmlChar * unicast = xmlGetProp(xmlIA,(const xmlChar*)"unicast");
     char packedUnicast[16];
     inet_pton6((const char *)unicast,packedUnicast);
     SmartPtr<TIPv6Addr> ptrUnicast = new TIPv6Addr(packedUnicast);

     // T1
     xmlChar * T1Str = xmlGetProp(xmlIA,(const xmlChar*)"T1");
     unsigned long T1 = atol((const char *)T1Str);

     // T2
     xmlChar * T2Str = xmlGetProp(xmlIA,(const xmlChar*)"T2");
     unsigned long T2 = atol((const char *)T2Str);
    
     // IAID
     xmlChar * IAIDStr = xmlGetProp(xmlIA,(const xmlChar*)"IAID");
     unsigned long IAID = atol((const char *)IAIDStr);

     // state
     xmlChar * stateStr = xmlGetProp(xmlIA,(const xmlChar*)"state");
     unsigned long state = atol((const char *)stateStr);

     // iface
     xmlChar * ifaceStr = xmlGetProp(xmlIA,(const xmlChar*)"iface");
     unsigned long iface = atol((const char *)ifaceStr);

     // now we've got all data, so create AddrIA
     SmartPtr<TAddrIA> ptrIA = new TAddrIA(iface, ptrUnicast, ptrDUID, T1, T2, IAID);

     xmlFree(DUIDStr);
     xmlFree(unicast);
     xmlFree(T1Str);
     xmlFree(T2Str);
     xmlFree(IAIDStr);
     xmlFree(stateStr);
     xmlFree(ifaceStr);
    
     // look for each address
     SmartPtr<TAddrAddr> ptrAddr;
     xmlNodePtr xmlAddr = xmlIA->xmlChildrenNode;
     while (xmlAddr) {
 	if (xmlAddr->type == XML_ELEMENT_NODE) {
 	    ptrAddr = parseAddrAddr(doc, xmlAddr, depth+2);
 	    ptrIA->addAddr(ptrAddr);
 	}
 	xmlAddr = xmlAddr->next;
     }

     return ptrIA;
//    return NULL;
}

SmartPtr<TAddrClient> TAddrMgr::parseAddrClient(xmlDocPtr doc, xmlNodePtr xmlClient, int depth)
{
     // DUID
     xmlChar *DUID;
     DUID = xmlGetProp(xmlClient,(const xmlChar*)"DUID");
     SmartPtr<TDUID> ptrDUID = new TDUID((char*)DUID);

     // create AddrClient
     SmartPtr<TAddrClient> ptrClient = new TAddrClient(ptrDUID);

     xmlFree(DUID);

     SmartPtr<TAddrIA> ptrIA;
     xmlNodePtr xmlIA = xmlClient->xmlChildrenNode;
     while (xmlIA) {
 	if (xmlIA->type == XML_ELEMENT_NODE) {
 	    ptrIA = parseAddrIA(doc, xmlIA, depth+2);
 	    ptrClient->addIA(ptrIA);
 	}
 	xmlIA = xmlIA->next;
     }
     return ptrClient;
//    return NULL;
}

void TAddrMgr::parseAddrMgr(xmlDocPtr doc,int depth)
{
     xmlNodePtr n;
     xmlNodePtr xmlClient;
     n = xmlDocGetRootElement(doc);

     xmlClient = n->xmlChildrenNode;

     SmartPtr<TAddrClient> ptrClient;

     while (xmlClient) {
 	if (xmlClient->type == XML_ELEMENT_NODE) {
 	    ptrClient = parseAddrClient(doc,xmlClient,depth+2);

 	    // append this client to clients list
 	    this->ClntsLst.append(ptrClient);
 	}
 	xmlClient = xmlClient->next;
     }
}
#endif

bool TAddrMgr::isDone() {
    return this->IsDone;
}

TAddrMgr::~TAddrMgr() {

}

// --------------------------------------------------------------------
// --- operators ------------------------------------------------------
// --------------------------------------------------------------------

ostream & operator<<(ostream & strum,TAddrMgr &x) {
    strum << "<AddrMgr>" << endl;
    strum << "  <timestamp>" << now() << "</timestamp>" << endl;
    x.print(strum);

    SmartPtr<TAddrClient> ptr;
    x.ClntsLst.first();

    while ( ptr = x.ClntsLst.get() ) {
        strum << *ptr;
    }

    strum << "</AddrMgr>" << endl;
    return strum;
}
