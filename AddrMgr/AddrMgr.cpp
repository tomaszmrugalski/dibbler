/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: AddrMgr.cpp,v 1.19 2005-09-20 20:10:51 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.18  2004/12/07 00:45:41  thomson
 * Clnt managers creation unified and cleaned up.
 *
 * Revision 1.17  2004/07/05 00:53:03  thomson
 * Various changes.
 *
 * Revision 1.16  2004/06/17 23:53:54  thomson
 * Server Address Assignment rewritten.
 *
 *
 * Revision 1.10  2004/04/10 12:18:00  thomson
 * Numerous fixes: LogName, LogMode options added, dns-servers changed to
 * dns-server, '' around strings are no longer needed.
 *
 * Revision 1.9  2004/04/09 22:33:11  thomson
 * dns-servers changed to option dns-server
 *
 * Revision 1.7  2004/04/06 23:09:48  thomson
 * dbStore was causing segfaults in rare occasions, so it is commented out.
 *
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
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

SmartPtr<TAddrClient> TAddrMgr::getClient(SmartPtr<TDUID> duid)
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
    unsigned long ts = LONG_MAX;
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
    unsigned long ts = LONG_MAX;
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
    unsigned long ts = LONG_MAX;
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
    return 1;
}

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
    SmartPtr<TAddrClient> ptr;

    x.ClntsLst.first();

    while ( ptr = x.ClntsLst.get() ) {
        strum << *ptr;
    }

    strum << "</AddrMgr>" << endl;
    return strum;
}
