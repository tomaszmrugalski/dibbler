	/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgReply.cpp,v 1.8 2007-05-04 17:22:49 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "ClntMsgReply.h"
#include "ClntMsg.h"

TClntMsgReply::TClntMsgReply(SmartPtr<TClntIfaceMgr> IfaceMgr, 
			     SmartPtr<TClntTransMgr> TransMgr, 
			     SmartPtr<TClntCfgMgr> CfgMgr, 
			     SmartPtr<TClntAddrMgr> AddrMgr,
			     int iface, SmartPtr<TIPv6Addr> addr)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,REPLY_MSG)
{
}

TClntMsgReply::TClntMsgReply(SmartPtr<TClntIfaceMgr> IfaceMgr, 
			     SmartPtr<TClntTransMgr> TransMgr, 
			     SmartPtr<TClntCfgMgr> CfgMgr, 
			     SmartPtr<TClntAddrMgr> AddrMgr,
			     int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr, AddrMgr, iface, addr,buf,bufSize)
{
}

void TClntMsgReply::answer(SmartPtr<TClntMsg> Reply) {
    // this should never happen. After receiving REPLY for e.g. REQUEST,
    // request->answer(reply) is called. Client nevers sends reply msg, so
    // this method will never be called.
}

void TClntMsgReply::doDuties() {
}


bool TClntMsgReply::check() {
    bool anonInfReq = ClntCfgMgr->anonInfRequest();
    return TClntMsg::check(!anonInfReq /* clientID mandatory */, true /* serverID mandatory */ );
}


string TClntMsgReply::getName() {
    return "REPLY";
}

TClntMsgReply::~TClntMsgReply() {
}
