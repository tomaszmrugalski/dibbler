/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgDecline.cpp,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *                                                                           
 */

#include "SrvMsgDecline.h"
#include "AddrClient.h"
#include "SrvOptServerIdentifier.h"

TSrvMsgDecline::TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			       SmartPtr<TSrvTransMgr> TransMgr,
			       SmartPtr<TSrvCfgMgr> CfgMgr,
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface,  SmartPtr<TIPv6Addr> addr)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,DECLINE_MSG)
{
    
}
    
TSrvMsgDecline::TSrvMsgDecline(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			       SmartPtr<TSrvTransMgr> TransMgr,
			       SmartPtr<TSrvCfgMgr> CfgMgr,
			       SmartPtr<TSrvAddrMgr> AddrMgr,
			       int iface, SmartPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,bufSize)
{
	pkt=NULL;
}

void TSrvMsgDecline::answer(SmartPtr<TMsg> Rep)
{
}

void TSrvMsgDecline::doDuties()
{
}

unsigned long TSrvMsgDecline::getTimeout()
{
	return 0;
}

bool TSrvMsgDecline::check()
{
    
//   Servers MUST discard any received Decline message that meets any of
//   the following conditions:
//    -  the message does not include a Server Identifier option.
    SmartPtr<TOpt> ptrOpt = getOption(OPTION_SERVERID);
    if (!ptrOpt)
	return false;

//   -  the contents of the Server Identifier option does not match the
//      server's identifier.
    SmartPtr<TSrvOptServerIdentifier> optSrvDUID = (Ptr*) ptrOpt;
    if ( !( *(SrvCfgMgr->getDUID()) == *(optSrvDUID->getDUID()) ) )
	return false;

//   -  the message does not include a Client Identifier option.
    ptrOpt = getOption(OPTION_CLIENTID);
    if (!ptrOpt)
	return false;

    return true;
}

TSrvMsgDecline::~TSrvMsgDecline()
{
}


string TSrvMsgDecline::getName() {
    return "DECLINE";
}
