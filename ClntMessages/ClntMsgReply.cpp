/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgReply.cpp,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "SmartPtr.h"
#include "ClntMsgReply.h"
#include "ClntMsg.h"

//##ModelId=3EC8AA2F0262
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

void TClntMsgReply::answer(SmartPtr<TMsg> Rep)
{
}

void TClntMsgReply::doDuties()
{
}


bool TClntMsgReply::check()
{
    //Clients MUST discard any received Reply message that meets any of the
    //following conditions:
    //-  the message does not include a Server Identifier option.
    if (!getOption(OPTION_SERVERID))
        return false;
    //-  the "transaction-id" field in the message does not match the value
    //   used in the original message. (should be checked before)
    
    //FIXME: If the client included a Client Identifier option in the original
    //message, the Reply message MUST include a Client Identifier option
    //and the contents of the Client Identifier option MUST match the DUID
    //of the client;OR, if the client did not include a Client Identifier
    //option in the original message, the Reply message MUST NOT include a
    //Client Identifier option.

    //Servers and relay agents MUST discard any received Reply messages. 
    return true;
}


string TClntMsgReply::getName() {
    return "REPLY";
}

TClntMsgReply::~TClntMsgReply()
{
}
