/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 * changes: Grzegorz Pluto   <g.pluto(at)u-r-b-a-n(dot)pl>
 *
 * released under GNU GPL v2 licence
 */

class TAddrMgr;
#ifndef ADDRMGR_H
#define ADDRMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "AddrClient.h"
#include "AddrIA.h"

#ifdef MOD_LIBXML2
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define ADDRDB_DTD (xmlChar*)"AddrMgr.dtd"
#endif

class TAddrMgr
{
  public:
    friend ostream & operator<<(ostream & strum,TAddrMgr &x);
    TAddrMgr(string addrdb, bool loadfile = false);
    virtual ~TAddrMgr();
    
    //--- Client container ---
    void addClient(SPtr<TAddrClient> x);
    void firstClient();
    SPtr<TAddrClient> getClient();
    SPtr<TAddrClient> getClient(SPtr<TDUID> duid);
    SPtr<TAddrClient> getClient(uint32_t SPI);
    SPtr<TAddrClient> getClient(SPtr<TIPv6Addr> leasedAddr);
    int countClient();
    bool delClient(SPtr<TDUID> duid);

    // --- prefix related ---
    virtual bool addPrefix(SPtr<TDUID> clntDuid, SPtr<TIPv6Addr> clntAddr,
			   int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
			   SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			   int length, bool quiet);
    virtual bool updatePrefix(SPtr<TDUID> duid , SPtr<TIPv6Addr> addr,
			      int iface, unsigned long IAID, unsigned long T1, unsigned long T2,
			      SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
			      int length, bool quiet);

    bool delPrefix(SPtr<TDUID> clntDuid, unsigned long IAID, SPtr<TIPv6Addr> prefix, bool quiet);
    bool prefixIsFree(SPtr<TIPv6Addr> prefix);

    //--- Time related methods ---
    unsigned long getT1Timeout();
    unsigned long getT2Timeout();
    unsigned long getPrefTimeout();
    unsigned long getValidTimeout();
    
    // --- backup/restore ---
    void dbLoad(const char * xmlFile);
    virtual void dump();
    bool isDone();
    bool restore;

#ifdef MOD_LIBXML2
    // database loading methods that use libxml2
    xmlDocPtr xmlLoad(const char * filename);
    SPtr<TAddrAddr> parseAddrAddr(xmlDocPtr doc, xmlNodePtr xmlAddr, int depth);
    SPtr<TAddrIA> libxml_parseAddrIA(xmlDocPtr doc, xmlNodePtr xmlIA, int depth);
    SPtr<TAddrClient> parseAddrClient(xmlDocPtr doc, xmlNodePtr xmlClient, int depth);
    void parseAddrMgr(xmlDocPtr doc,int depth);
#else
    // database loading methods that use internal loading routines
    bool xmlLoadBuiltIn(const char * xmlFile);
    SPtr<TAddrClient> parseAddrClient(FILE *f);
    SPtr<TAddrIA> parseAddrIA(FILE * f, int t1,int t2,int iaid,int iface);
    SPtr<TAddrIA> parseAddrPD(FILE * f, int t1,int t2,int iaid,int iface);
    SPtr<TAddrAddr> parseAddrAddr(char * buf,bool pd);
    SPtr<TAddrPrefix> parseAddrPrefix(char * buf,bool pd);
    SPtr<TAddrIA> parseAddrTA(FILE *f);
#endif

protected:
    virtual void print(ostream & out) = 0;
    bool addPrefix(SPtr<TAddrClient> client, SPtr<TDUID> duid , SPtr<TIPv6Addr> clntAddr,
		   int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
		   SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
		   int length, bool quiet);
    bool updatePrefix(SPtr<TAddrClient> client, SPtr<TDUID> duid , SPtr<TIPv6Addr> clntAddr,
		      int iface, unsigned long IAID, unsigned long T1, unsigned long T2, 
		      SPtr<TIPv6Addr> prefix, unsigned long pref, unsigned long valid,
		      int length, bool quiet);

    bool IsDone;
    List(TAddrClient) ClntsLst;
    string XmlFile;

    bool DeleteEmptyClient; // should the client without any IA, TA or PDs be deleted? (srv = yes, client = no)
};

#endif
