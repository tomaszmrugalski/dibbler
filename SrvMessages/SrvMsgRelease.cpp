/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgRelease.cpp,v 1.7 2008-08-29 00:07:35 thomson Exp $
 *
 */

#include "SrvMsgRelease.h"
#include "SrvOptServerIdentifier.h"
#include "AddrClient.h"

TSrvMsgRelease::TSrvMsgRelease(
	SPtr<TSrvIfaceMgr> IfMgr, 
	SPtr<TSrvTransMgr> TransMgr,
	SPtr<TSrvCfgMgr>   ConfMgr, 
	SPtr<TSrvAddrMgr>  AddrMgr, 
	int iface, 
	SPtr<TIPv6Addr> addr,
	char* buf,
	int bufSize)
    :TSrvMsg(IfMgr, TransMgr, ConfMgr, AddrMgr, iface, addr,buf,bufSize)
{
}

void TSrvMsgRelease::doDuties() {
}

unsigned long TSrvMsgRelease::getTimeout() {
    return 0;
}

bool TSrvMsgRelease::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID required */);
}

string TSrvMsgRelease::getName() {
    return "RELEASE";
}

TSrvMsgRelease::~TSrvMsgRelease() {
}
