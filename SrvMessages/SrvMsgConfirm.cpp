/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgConfirm.cpp,v 1.4 2006-08-21 21:33:20 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "AddrClient.h"
#include "SrvMsgConfirm.h"

TSrvMsgConfirm::TSrvMsgConfirm(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
			       SmartPtr<TSrvTransMgr> TransMgr, 
			       SmartPtr<TSrvCfgMgr> CfgMgr, 
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,buf,bufSize)
{
}

void TSrvMsgConfirm::doDuties() {
}

unsigned long TSrvMsgConfirm::getTimeout() {
	return 0;
}

bool TSrvMsgConfirm::check() {
    //Servers MUST discard any received Confirm messages that do not
    //include a Client Identifier option or that do include a Server
    //Identifier option. 
    if(!getOption(OPTION_CLIENTID))
        return false;
    if (getOption(OPTION_SERVERID))
        return false;
    return true;
}

TSrvMsgConfirm::~TSrvMsgConfirm() {
}

string TSrvMsgConfirm::getName() {
    return "CONFIRM";
}
