/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelTransMgr.h,v 1.3 2008-08-29 00:07:33 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005-01-13 22:45:55  thomson
 * Relays implemented.
 *
 * Revision 1.1  2005/01/11 22:53:36  thomson
 * Relay skeleton implemented.
 *
 *
 */

class TRelTransMgr;
#ifndef RELTRANSMGR_H
#define RELTRANSMGR_H

#include <iostream>
#include "SmartPtr.h"
#include "RelCfgIface.h"
#include "RelMsg.h"

#define RelTransMgr() (TRelTransMgr::instance())

class TRelTransMgr
{
    friend ostream & operator<<(ostream &strum, TRelTransMgr &x);
  public:
    static void instanceCreate(const std::string xmlFile);
    static TRelTransMgr& instance();

    ~TRelTransMgr();

    bool openSocket(SPtr<TRelCfgIface> confIface);

    bool doDuties();

    void relayMsg(SPtr<TRelMsg> msg);
    void relayMsgRepl(SPtr<TRelMsg> msg);
    void dump();

    bool isDone();
    void shutdown();
    
    char * getCtrlAddr();
    int    getCtrlIface();
    
  private:
    TRelTransMgr(const std::string xmlFile);
    static TRelTransMgr * Instance;
    string XmlFile;
    bool IsDone;

    int ctrlIface;
    char ctrlAddr[48];
};



#endif


