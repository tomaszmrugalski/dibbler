/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgInfRequest.cpp,v 1.7 2008-08-29 00:07:34 thomson Exp $
 *
 */

#include "SrvMsgInfRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvIfaceMgr.h"
#include "SrvMsgAdvertise.h"
#include "SrvOptServerIdentifier.h"

TSrvMsgInfRequest::TSrvMsgInfRequest(SPtr<TSrvIfaceMgr> IfaceMgr, 
				     SPtr<TSrvTransMgr> TransMgr,
				     SPtr<TSrvCfgMgr>   ConfMgr, 
				     SPtr<TSrvAddrMgr>  AddrMgr, 
				     int iface,  
				     SPtr<TIPv6Addr> addr,
				     char* buf,
				     int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,ConfMgr,AddrMgr,iface,addr,buf,bufSize) {
}

void TSrvMsgInfRequest::doDuties() {
    return;
}

bool TSrvMsgInfRequest::check() {
    /* client is not required (but is allowed) to include ClientID option, also ServerID is optional */
    return true;
}

unsigned long TSrvMsgInfRequest::getTimeout() {
    return 0;
}

string TSrvMsgInfRequest::getName() {
    return "INF-REQUEST";
}


TSrvMsgInfRequest::~TSrvMsgInfRequest(){
}
