/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: IfaceMgr.h,v 1.9 2008-08-29 00:07:30 thomson Exp $
 *
 */

class TIfaceMgr;
#ifndef IFACEMGR_H
#define IFACEMGR_H

#include "SmartPtr.h"
#include "Container.h"

#include "Iface.h"

class TMsg;
class TOpt;

class TIfaceMgr {
  public:
    friend ostream & operator <<(ostream & strum, TIfaceMgr &x);

    TIfaceMgr(string xmlFile, bool getIfaces);

    // ---Iface related---
    void firstIface();
    SPtr<TIfaceIface> getIface();
    SPtr<TIfaceIface> getIfaceByName(string name);
    SPtr<TIfaceIface> getIfaceByID(int id);
    SPtr<TIfaceIface> getIfaceBySocket(int fd);
    int countIface();

    // ---other---
    int select(unsigned long time, char *buf, int &bufsize, SPtr<TIPv6Addr> peer);
    string printMac(char * mac, int macLen);
    void dump();
    bool isDone();

    void notifyScripts(std::string scriptName, SPtr<TMsg> question, SPtr<TMsg> answer);

    ~TIfaceMgr();

 protected:
    static int addParam(char ** param, int offset, const char * value);
    static void freeParams(char ** param);
	virtual int optionToEnv(char **env, int envCnt, int& ipCnt, int& pdCnt, SPtr<TOpt> opt);

    string XmlFile;
    List(TIfaceIface) IfaceLst; //Interface list
    bool IsDone; 
};

#endif
