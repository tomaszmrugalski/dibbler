/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRenew.cpp,v 1.4 2006-08-21 21:33:20 thomson Exp $
 *
 */

#include "SrvMsgRenew.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptServerIdentifier.h"
#include "SrvCfgMgr.h"
#include "DHCPConst.h"
#include "AddrClient.h"

TSrvMsgRenew::TSrvMsgRenew(SmartPtr<TSrvIfaceMgr> IfaceMgr,
			   SmartPtr<TSrvTransMgr> TransMgr,
			   SmartPtr<TSrvCfgMgr> CfgMgr,
			   SmartPtr<TSrvAddrMgr> AddrMgr,
			   int iface,  SmartPtr<TIPv6Addr> addr,
			   char* buf, int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,bufSize)
{

}

void TSrvMsgRenew::doDuties()
{
}

unsigned long TSrvMsgRenew::getTimeout()
{
	return 0;
}

bool TSrvMsgRenew::check()
{
// 15.6. Renew Message
    
//    Servers MUST discard any received Renew message that meets any of the
//    following conditions:

//    -  the message does not include a Server Identifier option.
    SmartPtr<TSrvOptServerIdentifier> ptrServerDUID;
    ptrServerDUID = (Ptr*) this->getOption(OPTION_SERVERID);
    if (!ptrServerDUID)
	return false;

//    -  the contents of the Server Identifier option does not match the
//       server's identifier.
    if (!(*(SrvCfgMgr->getDUID()) == *(ptrServerDUID->getDUID()) ))
	return false;

//    -  the message does not include a Client Identifier option.
    SmartPtr<TSrvOptClientIdentifier> ptrClientDUID;
    ptrClientDUID = (Ptr*) this->getOption(OPTION_CLIENTID);
    if (!ptrClientDUID)
	return false;

    return true;
}

string TSrvMsgRenew::getName() {
    return "RENEW";
}

TSrvMsgRenew::~TSrvMsgRenew()
{
}
