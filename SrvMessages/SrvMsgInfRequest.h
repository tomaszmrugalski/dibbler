#ifndef SRVINFREQUEST_H_HEADER_INCLUDED_C1127952
#define SRVINFREQUEST_H_HEADER_INCLUDED_C1127952

#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvAddrMgr.h"
#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "IPv6Addr.h"

class TSrvMsgInfRequest : public TSrvMsg
{
  public:
    //##Documentation
    //## Po wybraniu pierwszego serwera z listy serwerów, tworzona jest
    //## wiadomoœæ REQUEST.
    TSrvMsgInfRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr>   ConfMgr, 
		   SmartPtr<TSrvAddrMgr>  AddrMgr, 
		   int iface);

    TSrvMsgInfRequest(SmartPtr<TSrvIfaceMgr> IfMgr, 
		   SmartPtr<TSrvTransMgr> TransMgr,
		   SmartPtr<TSrvCfgMgr>   ConfMgr, 
		   SmartPtr<TSrvAddrMgr>  AddrMgr, 
		   int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize);	

    void answer(SmartPtr<TMsg> msg);
    void doDuties();
    bool check();
    unsigned long getTimeout();
    ~TSrvMsgInfRequest();
  private:
    SmartPtr<TSrvAddrMgr> AddrMgr;
    TContainer< SmartPtr<TMsg> > BackupSrvLst;
};


#endif /* REQUEST_H_HEADER_INCLUDED_C1127952 */
