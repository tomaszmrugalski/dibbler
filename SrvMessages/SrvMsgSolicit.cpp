/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgSolicit.cpp,v 1.2 2004-06-20 17:25:07 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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

void TSrvMsgSolicit::answer(SmartPtr<TMsg> msg)
{
	//FIXME
}

void TSrvMsgSolicit::doDuties()
{
	//FIXME
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

void TSrvMsgSolicit::send()
{
	//FIXME:Here was division by zero & what if RT=0
	//		it's not accrptable. (FIXED)
	//	RT =(int) floor(0.5+2.0*RT+RT*(0.2*(double)rand()/(double)RAND_MAX-0.1));

	//FIXME:rand() is supposed to return number in range: (-0.1,0.1)
	//FIXME:it's intresting even if it is above MRT, as it was set in prev instruction
	//		it can be set again below this value, so in SOLICIT message ending condition
	//		should be changed a little bit. By the way there is no SOLICIT ending condition
	//		this transaction can end only after receiving an answer

	//if (MRT != 0 && RT>MRT) 
	//	RT = (int) floor(0.5+MRT + MRT*(0.2*(double)rand()/(double)RAND_MAX-0.1));
	//RC++;
	
	TMsg::send();
    
/*	if (PeerAddr)
		ClntIfaceMgr->sendUnicast(Iface,pkt,getSize(),PeerAddr);
	else
		ClntIfaceMgr->sendMulticast(Iface, pkt, getSize());

	LastTimeStamp = now();*/
	
}

TSrvMsgSolicit::~TSrvMsgSolicit()
{
	delete pkt;
    pkt=NULL;
}
