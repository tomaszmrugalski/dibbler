#include "SmartPtr.h"
#include "AddrClient.h"
#include "SrvMsgConfirm.h"

TSrvMsgConfirm::TSrvMsgConfirm(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
			       SmartPtr<TSrvTransMgr> TransMgr, 
			       SmartPtr<TSrvCfgMgr>	CfgMgr, 
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,CONFIRM_MSG)
{
}

TSrvMsgConfirm::TSrvMsgConfirm(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
			       SmartPtr<TSrvTransMgr> TransMgr, 
			       SmartPtr<TSrvCfgMgr> CfgMgr, 
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,buf,bufSize)
{
	pkt=NULL;
}

void TSrvMsgConfirm::answer(SmartPtr<TMsg> Rep)
{
}

void TSrvMsgConfirm::doDuties()
{
}

unsigned long TSrvMsgConfirm::getTimeout()
{
	return 0;
}

bool TSrvMsgConfirm::check()
{
    //Servers MUST discard any received Confirm messages that do not
    //include a Client Identifier option or that do include a Server
    //Identifier option. 
    if(!getOption(OPTION_CLIENTID))
        return false;
    if (getOption(OPTION_SERVERID))
        return false;
    return true;
}

TSrvMsgConfirm::~TSrvMsgConfirm()
{
}
