#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvMsgRebind.h"
#include "AddrClient.h"

TSrvMsgRebind::TSrvMsgRebind(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr, 
		SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, 
		 SmartPtr<TIPv6Addr> addr)
		:TSrvMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,REBIND_MSG)
{
}

TSrvMsgRebind::TSrvMsgRebind(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
		SmartPtr<TSrvTransMgr> TransMgr, 
		SmartPtr<TSrvCfgMgr> CfgMgr, 
		SmartPtr<TSrvAddrMgr> AddrMgr,
		 int iface, 
         SmartPtr<TIPv6Addr> addr,
		 char* buf,
		 int bufSize)
		:TSrvMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,buf,bufSize)
{
}

void TSrvMsgRebind::answer(SmartPtr<TMsg> Rep)
{
}

void TSrvMsgRebind::doDuties()
{
}

unsigned long TSrvMsgRebind::getTimeout()
{
	return 0;
}

bool TSrvMsgRebind::check()
{

  //   Servers MUST discard any received Rebind messages that do not include
  // a Client Identifier option or that do include a Server Identifier
  // option.
    SmartPtr<TOpt> ptrOpt = (Ptr*) getOption(OPTION_CLIENTID);
    if (!ptrOpt) return false;
    
    ptrOpt = (Ptr*) getOption(OPTION_SERVERID);
    if (ptrOpt) return false;

    return true;
}

TSrvMsgRebind::~TSrvMsgRebind()
{
}
