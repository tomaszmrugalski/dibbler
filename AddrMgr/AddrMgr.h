

class TAddrMgr;
#ifndef ADDRMGR_H
#define ADDRMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "AddrClient.h"
#include "AddrIA.h"
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define ADDRDB_DTD (xmlChar*)"AddrMgr.dtd"

class TAddrMgr
{
  public:
    friend ostream & operator<<(ostream & strum,TAddrMgr &x);
    TAddrMgr(string addrdb, bool loadfile = false);
    
    //--- Client container ---
    void addClient(SmartPtr<TAddrClient> x);
    void firstClient();
    SmartPtr<TAddrClient> getClient();
    SmartPtr<TAddrClient> getClient(SmartPtr<TDUID> duid);
    int countClient();
    bool delClient(SmartPtr<TDUID> duid);

    //--- Time related methods ---
    long getT1Timeout();
    long getT2Timeout();
    long getPrefTimeout();
    long getValidTimeout();
    unsigned long getAddrCount(SmartPtr<TDUID> duid, int iface);
    
    // --- backup/restore ---
    virtual void dbLoad();
    virtual void dbStore();
    xmlDocPtr xmlLoad(const char * filename);
    SmartPtr<TAddrAddr> parseAddrAddr(xmlDocPtr doc, xmlNodePtr xmlAddr, int depth);
    SmartPtr<TAddrIA> parseAddrIA(xmlDocPtr doc, xmlNodePtr xmlIA, int depth);
    SmartPtr<TAddrClient> parseAddrClient(xmlDocPtr doc, xmlNodePtr xmlClient, int depth);
    void parseAddrMgr(xmlDocPtr doc,int depth);

protected:
    TContainer< SmartPtr<TAddrClient> > ClntsLst;
    string dbfile;
};

#endif
