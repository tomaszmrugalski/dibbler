/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgSolicit.cpp,v 1.6 2008-08-29 00:07:35 thomson Exp $
 *
 */

#include "SrvMsgSolicit.h"
#include "Msg.h"
#include "SmartPtr.h"

#include "SrvCfgMgr.h"
#include "SrvIfaceMgr.h"
#include "SrvTransMgr.h"
#include "SrvAddrMgr.h"
#include "SrvMsg.h"
#include "AddrClient.h"
#include <cmath>

TSrvMsgSolicit::TSrvMsgSolicit(SmartPtr<TSrvIfaceMgr>	ifaceMgr, 
			       SmartPtr<TSrvTransMgr>	transMgr,
			       SmartPtr<TSrvCfgMgr>	cfgMgr,
			       SmartPtr<TSrvAddrMgr>	addrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(ifaceMgr,transMgr,cfgMgr,addrMgr,iface,addr,buf,bufSize) {
}

void TSrvMsgSolicit::doDuties() {
    // this function should not be called on the server side
}

string TSrvMsgSolicit::getName() {
    return "SOLICIT";
}

bool TSrvMsgSolicit::check() {
    return TSrvMsg::check(true /* ClientID required */, false /* ServerID not allowed */);
}

unsigned long TSrvMsgSolicit::getTimeout() {
	return 0;
}

TSrvMsgSolicit::~TSrvMsgSolicit() {

}
