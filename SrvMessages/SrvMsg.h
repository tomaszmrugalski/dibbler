#ifndef SRVMSG_H_
#define SRVMSG_H_

#include "Msg.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvCfgMgr.h"
#include "SrvAddrMgr.h"
#include "IPv6Addr.h"
#include "SrvOptOptionRequest.h"
#include "SmartPtr.h"

class TSrvMsg : public TMsg
{
public:
	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		int iface,  SmartPtr<TIPv6Addr> addr, char* buf,  int bufSize);
	
	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		int iface, SmartPtr<TIPv6Addr> addr,  int msgType);

	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr,
		int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID);

	TSrvMsg(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr,
		SmartPtr<TSrvAddrMgr> AddrMgr);
	
	void answer(SmartPtr<TMsg> answer);

    SmartPtr<TSrvTransMgr>  SrvTransMgr;
    SmartPtr<TSrvAddrMgr>   SrvAddrMgr;
    SmartPtr<TSrvCfgMgr>    SrvCfgMgr;
    SmartPtr<TSrvIfaceMgr>  SrvIfaceMgr;
    
    bool appendRequestedOptions(SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, 
        int iface, SmartPtr<TSrvOptOptionRequest> reqOpt);

    // maximum retransmission timeout
    unsigned long MRT;

    unsigned long getTimeout();
    void doDuties();
    void send();
protected:
//    void getFreeAddressesForClient(    
//        SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > &clntAllClasses,
//        SmartPtr<TContainer<SmartPtr<TSrvCfgAddrClass> > > &clntClasses,
//        long* &clntFreeAddr, long &totalFreeAddresses, 
//        SmartPtr<TDUID> duid, SmartPtr<TIPv6Addr> addr, int iface, bool rapid=false);
private:
    // timestamp of first message transmission
    unsigned long FirstTimeStamp;

    void setAttribs(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		    SmartPtr<TSrvTransMgr> TransMgr, 
		    SmartPtr<TSrvCfgMgr> CfgMgr,
		    SmartPtr<TSrvAddrMgr> AddrMgr);
};

#endif
