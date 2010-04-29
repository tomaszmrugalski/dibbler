/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvTransMgr.h,v 1.12 2008-10-12 20:10:25 thomson Exp $
 *
 */

#ifndef SRVTRANSMGR_H
#define SRVTRANSMGR_H

#include <string>
#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

#define SrvTransMgr() (TSrvTransMgr::instance())

class TSrvTransMgr
{
    friend ostream & operator<<(ostream &strum, TSrvTransMgr &x);
  public:
    static void instanceCreate(const std::string config);
    static TSrvTransMgr &instance();

    bool openSocket(SPtr<TSrvCfgIface> confIface);
    SPtr<TMsg> getCurrentRequest();

    long getTimeout();
    void relayMsg(SPtr<TSrvMsg> msg);
    void doDuties();
    void dump();

    bool isDone();
    void shutdown();

    char * getCtrlAddr();
    int    getCtrlIface();

    SPtr<TSrvMsg> requestMsg;
  private:
    TSrvTransMgr(string xmlFile);
    ~TSrvTransMgr();

    string XmlFile;
    List(TSrvMsg) MsgLst;
    bool IsDone;

    int ctrlIface;
    char ctrlAddr[48];

    static TSrvTransMgr * Instance;
};



#endif


