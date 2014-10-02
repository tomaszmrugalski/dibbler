/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
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
    friend std::ostream & operator<<(std::ostream &strum, TRelTransMgr &x);
  public:
    static void instanceCreate(const std::string& xmlFile);
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

protected:
    TRelTransMgr(const std::string& xmlFile);
    static TRelTransMgr * Instance;

    SPtr<TOpt> getClientLinkLayerAddr(SPtr<TRelMsg> msg);
    SPtr<TOpt> getLinkAddrFromSrcAddr(SPtr<TRelMsg> msg);
    SPtr<TOpt> getLinkAddrFromDuid(SPtr<TOpt> duid_opt);

  private:
    std::string XmlFile;
    bool IsDone;
    int ctrlIface;
    char ctrlAddr[48];
};



#endif
