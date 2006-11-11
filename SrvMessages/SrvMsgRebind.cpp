/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRebind.cpp,v 1.5 2006-11-11 06:56:27 thomson Exp $
 *
 */
#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvMsgRebind.h"
#include "AddrClient.h"

TSrvMsgRebind::TSrvMsgRebind(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
			     SmartPtr<TSrvTransMgr> TransMgr, 
			     SmartPtr<TSrvCfgMgr> CfgMgr, 
			     SmartPtr<TSrvAddrMgr> AddrMgr,
			     int iface, SmartPtr<TIPv6Addr> addr,
			     char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,buf,bufSize) {
}

void TSrvMsgRebind::doDuties() {
}

unsigned long TSrvMsgRebind::getTimeout() {
    return 0;
}

bool TSrvMsgRebind::check() {
    return TSrvMsg::check(true /* ClientID required */, false /* ServerID not allowed */);
}

string TSrvMsgRebind::getName() {
    return "REBIND";
}

TSrvMsgRebind::~TSrvMsgRebind() {
}
