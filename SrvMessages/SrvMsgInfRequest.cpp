/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgInfRequest.cpp,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
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

// opts - options list WITHOUT serverDUID
TSrvMsgInfRequest::TSrvMsgInfRequest(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
				     SmartPtr<TSrvTransMgr> TransMgr,
				     SmartPtr<TSrvCfgMgr>   CfgMgr, 
				     SmartPtr<TSrvAddrMgr> AddrMgr, 
				     int iface)
    :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface, SmartPtr<TIPv6Addr>()/*NULL*/,REQUEST_MSG)
{
    MRT = REQ_MAX_RT;
    Iface=iface;
    IsDone=false;
}

TSrvMsgInfRequest::TSrvMsgInfRequest(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                               SmartPtr<TSrvTransMgr> TransMgr,
                               SmartPtr<TSrvCfgMgr>   ConfMgr, 
                               SmartPtr<TSrvAddrMgr>  AddrMgr, 
                               int iface,  SmartPtr<TIPv6Addr> addr,
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
    SmartPtr<TOptServerIdentifier> option;
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
