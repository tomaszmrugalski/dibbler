/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgRequest.cpp,v 1.6 2006-08-21 21:33:21 thomson Exp $
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
    SmartPtr<TSrvOptServerIdentifier> option;
    int clntCnt=0, srvCnt =0;
    SmartPtr<TSrvOptServerIdentifier> srvDUID;
    Options.first();
    while (option = (Ptr*) Options.get() ) 
    {
        if (option->getOptType() == OPTION_CLIENTID)
            clntCnt++;
        if (option->getOptType() == OPTION_SERVERID)
        {
            srvCnt++; 
            srvDUID=(Ptr*)option;
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
