/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelTransMgr.h,v 1.2 2005-01-13 22:45:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:36  thomson
 * Relay skeleton implemented.
 *
 *
 */

class TRelTransMgr;
#ifndef RELTRANSMGR_H
#define RELTRANSMGR_H

#include <iostream>
#include "RelCommon.h"
#include "SmartPtr.h"
#include "RelCfgIface.h"
#include "RelMsg.h"

using namespace std;

class TRelTransMgr
{
    friend ostream & operator<<(ostream &strum, TRelTransMgr &x);
  public:
    TRelTransMgr(TCtx * ctx, string xmlFile);
    ~TRelTransMgr();

    bool openSocket(SmartPtr<TRelCfgIface> confIface);

    bool doDuties();

    void relayMsg(SmartPtr<TRelMsg> msg);
    void relayMsgRepl(SmartPtr<TRelMsg> msg);
    void dump();

    bool isDone();
    void shutdown();
    
    char * getCtrlAddr();
    int    getCtrlIface();
    
  private:
    TCtx * Ctx;
    string XmlFile;
    bool IsDone;

    int ctrlIface;
    char ctrlAddr[48];
};



#endif


