/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski  <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: AddrMgr.h,v 1.4 2004-04-11 18:04:53 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/03/28 19:58:15  thomson
 * no message
 *
 */

class TAddrMgr;
#ifndef ADDRMGR_H
#define ADDRMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "AddrClient.h"
#include "AddrIA.h"

#ifdef LIBXML2
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define ADDRDB_DTD (xmlChar*)"AddrMgr.dtd"
#endif

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

#ifdef LIBXML2
    xmlDocPtr xmlLoad(const char * filename);
    SmartPtr<TAddrAddr> parseAddrAddr(xmlDocPtr doc, xmlNodePtr xmlAddr, int depth);
    SmartPtr<TAddrIA> parseAddrIA(xmlDocPtr doc, xmlNodePtr xmlIA, int depth);
    SmartPtr<TAddrClient> parseAddrClient(xmlDocPtr doc, xmlNodePtr xmlClient, int depth);
    void parseAddrMgr(xmlDocPtr doc,int depth);
#endif

protected:
    TContainer< SmartPtr<TAddrClient> > ClntsLst;
    string dbfile;
};

#endif
