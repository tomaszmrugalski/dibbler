class TIfaceMgr;
#ifndef IFACEMGR_H
#define IFACEMGR_H

#include "SmartPtr.h"
#include "Container.h"

#include "Iface.h"

/*
 * InterfaceManager - provides access to all network interfaces present in the system
 * @date 2003-10-26
 * @author Tomasz Mrugalski <admin@klub.com.pl>
 * @licence GNU GPL v2 or later
 */

class TIfaceMgr {
  public:
    friend ostream & operator <<(ostream & strum, TIfaceMgr &x);

    TIfaceMgr(string xmlFile, bool getIfaces);

    // ---Iface related---
    void firstIface();
    SmartPtr<TIfaceIface> getIface();
    SmartPtr<TIfaceIface> getIfaceByName(string name);
    SmartPtr<TIfaceIface> getIfaceByID(int id);
    SmartPtr<TIfaceIface> getIfaceBySocket(int fd);
    int countIface();

    // ---other---
    int select(unsigned long time, char *buf, int &bufsize, SmartPtr<TIPv6Addr> peer);
    string printMac(char * mac, int macLen);
    void dump();
    bool isDone();

    ~TIfaceMgr();

 protected:
    string XmlFile;

    List(TIfaceIface) IfaceLst; //Interface list

    bool IsDone; 
};

#endif
