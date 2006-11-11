/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRenew.cpp,v 1.5 2006-11-11 06:56:27 thomson Exp $
 *
 */

#include "SrvMsgRenew.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptServerIdentifier.h"
#include "SrvCfgMgr.h"
#include "DHCPConst.h"

TSrvMsgRenew::TSrvMsgRenew(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			   SmartPtr<TSrvTransMgr> TransMgr,
			   SmartPtr<TSrvCfgMgr> CfgMgr,
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   int iface,  SmartPtr<TIPv6Addr> addr,
			   char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,bufSize) {

}

void TSrvMsgRenew::doDuties() {
}

unsigned long TSrvMsgRenew::getTimeout() {
    return 0;
}

bool TSrvMsgRenew::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID required */);
}

string TSrvMsgRenew::getName() {
    return "RENEW";
}

TSrvMsgRenew::~TSrvMsgRenew() {
}
