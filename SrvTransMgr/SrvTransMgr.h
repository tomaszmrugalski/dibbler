class TSrvTransMgr;
#ifndef SRVTRANSMGR_H
#define SRVTRANSMGR_H

#include "SmartPtr.h"
#include "Container.h"
#include "Opt.h"
#include "Msg.h"
#include "SrvIfaceMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"

class TSrvTransMgr
{
  public:
    TSrvTransMgr(SmartPtr<TSrvIfaceMgr> ifaceMgr,
		 string newconf, string oldconf);

    long getTimeout();
    void relayMsg(SmartPtr<TMsg> msg);
    void doDuties();

    bool isDone();
    void shutdown();
    
	char * getCtrlAddr();
	int    getCtrlIface();
    
    void setThat(SmartPtr<TSrvTransMgr> that);
  private:
    TContainer< SmartPtr<TMsg> > MsgLst;
    bool IsDone;

    SmartPtr<TSrvIfaceMgr> IfaceMgr;
    SmartPtr<TSrvTransMgr> That;
    SmartPtr<TSrvCfgMgr>  CfgMgr;
    SmartPtr<TSrvAddrMgr>  AddrMgr;

	int ctrlIface;
	char ctrlAddr[48];

};



#endif


