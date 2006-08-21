/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgInfRequest.cpp,v 1.5 2006-08-21 21:33:20 thomson Exp $
 *
 */

#include "SrvMsgInfRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvIfaceMgr.h"
#include "SrvMsgAdvertise.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "AddrClient.h"

TSrvMsgInfRequest::TSrvMsgInfRequest(SPtr<TSrvIfaceMgr> IfaceMgr, 
				     SPtr<TSrvTransMgr> TransMgr,
				     SPtr<TSrvCfgMgr>   ConfMgr, 
				     SPtr<TSrvAddrMgr>  AddrMgr, 
				     int iface,  
				     SPtr<TIPv6Addr> addr,
				     char* buf,
				     int bufSize)
    :TSrvMsg(IfaceMgr,TransMgr,ConfMgr,AddrMgr,iface,addr,buf,bufSize)
{
}

void TSrvMsgInfRequest::doDuties()
{
    return;
}

bool TSrvMsgInfRequest::check()
{
    SmartPtr<TSrvOptServerIdentifier> option;
    int clntCnt=0;
    Options.first();
    while (option = (Ptr*) Options.get() ) 
    {
        if (option->getOptType() == OPTION_CLIENTID)
            clntCnt++;
    }
    if (clntCnt!=1) return false;
    return true;
}

unsigned long TSrvMsgInfRequest::getTimeout()
{
    return 0;
}

string TSrvMsgInfRequest::getName() {
    return "INF-REQUEST";
}


TSrvMsgInfRequest::~TSrvMsgInfRequest()
{

}
