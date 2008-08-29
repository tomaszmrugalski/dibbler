/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *          Micha³ Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: AddrMgr.cpp,v 1.37 2008-08-29 00:07:26 thomson Exp $
 *
 */

#include <iostream>
#include <fstream>
#include "Portable.h"
#include <limits.h>
#include "AddrMgr.h"
#include "AddrClient.h"
#include "DHCPConst.h"
#include "Logger.h"

TAddrMgr::TAddrMgr(string xmlFile, bool loadfile)
{
    this->IsDone = false;
    this->XmlFile = xmlFile;
    
    if (loadfile) {
	dbLoad(CLNTADDRMGR_FILE);
    } else {
	Log(Debug) << "Skipping database loading." << LogEnd;
    }

    DeleteEmptyClient = true;
}

void TAddrMgr::dbLoad(const char * xmlFile)
{

#ifdef MOD_LIBXML2
    Log(Debug) << "Loading " << xmlFile << " (using libxml2)." << LogEnd;
     xmlDocPtr root;
     root = xmlLoad(xmlFile);
     if (!root) {
 	Log(Error) << "File loading has failed." << LogEnd;
	return;
     }
     this->parseAddrMgr(root,0);
     xmlFreeDoc(root);
#else
     Log(Info) << "Loading old address database (" << xmlFile << "), using built-in routines." << LogEnd;
     xmlLoadBuiltIn(xmlFile);
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
// --- XML-related methods (libxml2) ----------------------------------
// --------------------------------------------------------------------
#ifdef MOD_LIBXML2
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
 	//return NULL;
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
     SmartPtr<TIPv6Addr> ptrUnicast;
     if (unicast) {
       inet_pton6((const char *)unicast,packedUnicast);
       ptrUnicast = new TIPv6Addr(packedUnicast);
     }

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
	    ptrAddr->setTentative(TENTATIVE_NO);
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
#else
// --------------------------------------------------------------------
// --- XML-related methods (built-in) ---------------------------------
// --------------------------------------------------------------------
bool TAddrMgr::xmlLoadBuiltIn(const char * xmlFile)
{
    SPtr<TAddrClient> clnt;
    bool AddrClient = false;

    char buf[256];

    FILE * f;
    if (!(f = fopen(xmlFile,"r"))) {
        Log(Warning) << "Unable to open " << xmlFile << "." << LogEnd;
        return false;
    }

    while (!feof(f)) {
      fgets(buf, 255, f);
	if (strstr(buf,"<AddrMgr>")) {
	    AddrClient = true;
	    continue;
	}
	if (AddrClient && strstr(buf,"<AddrClient")) {
	  clnt = parseAddrClient(f);
	  continue;
	}

	if (strstr(buf,"</AddrMgr>")) {
	    break;
	}
    }
    fclose(f);

    if (clnt) {
	Log(Notice) << "Address database loaded successfuly." << LogEnd;
	ClntsLst.append(clnt);
	return true;
    } 
    
    return false;
}

SPtr<TAddrClient> TAddrMgr::parseAddrClient(FILE *f)
{
    char buf[256];
    char * x = 0;
    SPtr<TAddrClient> clnt = 0;
    SPtr<TDUID> duid = 0;
    SPtr<TAddrIA> ia = 0;

    while (!feof(f)) {
	fgets(buf,255,f);
	if (strstr(buf,"<duid")) {
	    x = strstr(buf,">")+1;
	    x = strstr(x,"</duid>");
	    if (x)
		*x = 0; // remove trailing xml tag
	    duid = new TDUID(strstr(buf,">")+1);
	    clnt = new TAddrClient(duid);
	    // Log(Debug) << "New address loaded from file created: duid=" << duid->getPlain() << LogEnd;
	    if (ia = parseAddrIA(f)) {
		clnt->addIA(ia);
	    }
	    // FIXME: support for more than one IA

	    // FIXME: support for PD
	    continue;
	}
	if (strstr(buf,"</AddrClient>"))
	    break;
    }

    // FIXME: add some extra checks here
    return clnt;
}

SPtr<TAddrIA> TAddrMgr::parseAddrIA(FILE * f)
{
    // IA paramteres
    char buf[256];
    char * x = 0;
    int t1 = 0, t2 = 0, iaid = 0, iface = 0;
    SPtr<TAddrIA> ia = 0;
    SPtr<TAddrAddr> addr;
    SPtr<TDUID> duid;

    while (!feof(f)) {
	fgets(buf,255,f);
	if (strstr(buf,"<AddrIA")) {
	    if ((x=strstr(buf,"T1"))) {
		t1=atoi(x+4);
	    }
	    if ((x=strstr(buf,"T2"))) {
		t2=atoi(x+4);
	    }
	    if ((x=strstr(buf,"IAID"))) {
		iaid=atoi(x+6);
	    }
	    if ((x=strstr(buf,"iface"))) {
		iface=atoi(x+7);
	    }

	    if (t1!=0 && t2!=0 && iaid!=0 && iface!=0) {
		Log(Debug) << "Loaded IA from a file: t1=" << t1 << ", t2="<< t2 << ",iaid=" << iaid << ", iface=" << iface << LogEnd;
		duid = 0; // don't use old DUID
	    }
	    continue;
	}
	if (strstr(buf,"duid")) {
	    char * x;
	    x = strstr(buf,">")+1;
	    x = strstr(x,"</duid>");
	    if (x)
		*x = 0; // remove trailing xml tag
	    duid = new TDUID(strstr(buf,">")+1);
	    // Log(Debug) << "Parsed IA: duid=" << duid->getPlain() << LogEnd;

	    ia = new TAddrIA(iface, 0, duid, t1,t2, iaid);
	    ia->setState(STATE_CONFIRMME);
	    continue;
	}
	if (strstr(buf,"<AddrAddr")) {
	    addr = parseAddrAddr(buf);
	    if (ia && addr) {
		ia->addAddr(addr);
		addr->setTentative(TENTATIVE_NO);
	    }
	}
	if (strstr(buf,"</AddrIA>"))
	    break;

    }
    if (ia)
	ia->setTentative();
    return ia;
}

SPtr<TAddrAddr> TAddrMgr::parseAddrAddr(char * buf)
{
    // address parameters
    unsigned long timestamp, pref=DHCPV6_INFINITY, valid=DHCPV6_INFINITY, prefix = CLIENT_DEFAULT_PREFIX_LENGTH;
    SPtr<TIPv6Addr> addr = 0;
    SPtr<TAddrAddr> addraddr;
    char * x;

    

    if (strstr(buf, "<AddrAddr")) {
	timestamp=pref=valid=0;
	addr = 0;
	if ((x=strstr(buf,"timestamp"))) {
	    timestamp = atoi(x+11);
	}
	if ((x=strstr(buf,"pref="))) {
	    pref = atoi(x+6);
	}
	if ((x=strstr(buf,"valid"))) {
	    valid = atoi(x+7);
	}
	if ((x=strstr(buf,"prefix="))) {
	    prefix = atoi(x+8);
	}
	if ((x=strstr(buf,">"))) {
	    x = strstr(x, "</AddrAddr>");
	    if (x)
		*x = 0;
	    addr = new TIPv6Addr(strstr(buf,">")+1, true);
	    Log(Debug) << "Parsed addr=" << addr->getPlain() << ", pref=" << pref << ", valid=" << valid << ",ts=" << timestamp << LogEnd;

	}
	if (addr && timestamp && pref && valid) {
	    addraddr = new TAddrAddr(addr, pref, valid, prefix);
	    addraddr->setTimestamp(timestamp);
	}
    }
    return addraddr;
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
