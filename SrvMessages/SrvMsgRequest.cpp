/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgRequest.cpp,v 1.3 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 *                                                                           
 */

#include "SrvMsgRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvIfaceMgr.h"
#include "SrvMsgAdvertise.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "AddrClient.h"

// opts - options list WITHOUT serverDUID
TSrvMsgRequest::TSrvMsgRequest(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                               SmartPtr<TSrvTransMgr> TransMgr,
                               SmartPtr<TSrvCfgMgr>   CfgMgr, 
                               SmartPtr<TSrvAddrMgr> AddrMgr, 
                               int iface)
                               :TSrvMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,
					SmartPtr<TIPv6Addr>(),REQUEST_MSG)
{
    MRT = REQ_MAX_RT;
    Iface=iface;
    IsDone=false;
}

TSrvMsgRequest::TSrvMsgRequest(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                               SmartPtr<TSrvTransMgr> TransMgr,
                               SmartPtr<TSrvCfgMgr>   ConfMgr, 
                               SmartPtr<TSrvAddrMgr>  AddrMgr, 
                               int iface,  SmartPtr<TIPv6Addr> addr,
                               char* buf,
                               int bufSize)
                               :TSrvMsg(IfaceMgr,TransMgr,ConfMgr,AddrMgr,iface,addr,buf,bufSize)
{
}

void TSrvMsgRequest::doDuties()
{
    return;
}

bool TSrvMsgRequest::check()
{
    SmartPtr<TOptServerIdentifier> option;
    int clntCnt=0, srvCnt =0;
    SmartPtr<TOptServerIdentifier> srvDUID;
    Options.first();
    while (option = (Ptr*) Options.get() ) 
    {
        if (option->getOptType() == OPTION_CLIENTID)
            clntCnt++;
        if (option->getOptType() == OPTION_SERVERID)
        {
            srvCnt++; 
            srvDUID=option;
        };
    }
    if (clntCnt!=1) return false;
    if (srvCnt!=1) 
        return false;
    else
    {
        if (!((*srvDUID->getDUID())==(*SrvCfgMgr->getDUID())))
            return false;
    };
    return true;
}

unsigned long TSrvMsgRequest::getTimeout()
{
    return 0;
}

TSrvMsgRequest::~TSrvMsgRequest()
{

}

string TSrvMsgRequest::getName() {
    return "REQUEST";
}
