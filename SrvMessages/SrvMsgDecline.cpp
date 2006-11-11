/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgDecline.cpp,v 1.5 2006-11-11 06:56:27 thomson Exp $
 *                                                                           
 */

#include "SrvMsgDecline.h"
#include "AddrClient.h"
#include "SrvOptServerIdentifier.h"
    
TSrvMsgDecline::TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			       SmartPtr<TSrvTransMgr> TransMgr,
			       SmartPtr<TSrvCfgMgr> CfgMgr,
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,bufSize) {
}

void TSrvMsgDecline::doDuties() {
}

unsigned long TSrvMsgDecline::getTimeout() {
    return 0;
}

bool TSrvMsgDecline::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID mandatory */);
}

TSrvMsgDecline::~TSrvMsgDecline() {
}


string TSrvMsgDecline::getName() {
    return "DECLINE";
}
