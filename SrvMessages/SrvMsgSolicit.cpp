/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgSolicit.cpp,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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
    :TSrvMsg(ifaceMgr,transMgr,cfgMgr,addrMgr,iface,addr,buf,bufSize)
{
}

void TSrvMsgSolicit::doDuties() {
    // this function should not be called on the server side
}

string TSrvMsgSolicit::getName() {
    return "SOLICIT";
}

bool TSrvMsgSolicit::check()
{
	SmartPtr<TOpt> option;
	int cnt=0;
	Options.first();
	while (option = Options.get() ) 
	{
		if (option->getOptType() == OPTION_CLIENTID)
			cnt++;
		if (option->getOptType() == OPTION_SERVERID)
			return false;
	}
		if (cnt!=1) return false;
	return true;
}

unsigned long TSrvMsgSolicit::getTimeout()
{
	return 0;
}

TSrvMsgSolicit::~TSrvMsgSolicit()
{
	delete pkt;
    pkt=NULL;
}
