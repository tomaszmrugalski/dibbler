/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                        
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgRequest.cpp,v 1.8 2006-11-11 06:56:27 thomson Exp $
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
    :TSrvMsg(IfaceMgr,TransMgr,ConfMgr,AddrMgr,iface,addr,buf,bufSize) {
}

void TSrvMsgRequest::doDuties() {
    return;
}

bool TSrvMsgRequest::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID required */);
}

unsigned long TSrvMsgRequest::getTimeout() {
    return 0;
}

TSrvMsgRequest::~TSrvMsgRequest() {

}

string TSrvMsgRequest::getName() {
    return "REQUEST";
}
