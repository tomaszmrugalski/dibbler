/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvTransMgr.h,v 1.7 2005-01-12 00:10:05 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/01/08 16:52:04  thomson
 * Relay support implemented.
 *
 * Revision 1.5  2004/12/07 00:45:10  thomson
 * Manager creation unified and cleaned up.
 *
 * Revision 1.4  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 * Revision 1.3  2004/09/03 20:58:36  thomson
 * *** empty log message ***
 *
 *
 */

class TSrvTransMgr;
#ifndef SRVTRANSMGR_H
#define SRVTRANSMGR_H

#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "SrvMsg.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

class TSrvTransMgr
{
    friend ostream & operator<<(ostream &strum, TSrvTransMgr &x);
  public:
    TSrvTransMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr,
		 SmartPtr<TSrvAddrMgr> addrMgr,
		 SmartPtr<TSrvCfgMgr> cfgMgr,
		 string xmlFile);
    ~TSrvTransMgr();

    bool openSocket(SmartPtr<TSrvCfgIface> confIface);

    long getTimeout();
    void relayMsg(SmartPtr<TSrvMsg> msg);
    void doDuties();
    void dump();

    bool isDone();
    void shutdown();
    
    char * getCtrlAddr();
    int    getCtrlIface();
    
    void setThat(SmartPtr<TSrvTransMgr> that);
  private:
    string XmlFile;
    List(TSrvMsg) MsgLst;
    bool IsDone;

    SmartPtr<TSrvIfaceMgr> IfaceMgr;
    SmartPtr<TSrvTransMgr> That;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TSrvAddrMgr>  AddrMgr;

    int ctrlIface;
    char ctrlAddr[48];
};



#endif


