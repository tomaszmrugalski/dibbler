/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *
 * released under GNU GPL v2 or later licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQuery.cpp,v 1.1 2007-11-01 08:10:34 thomson Exp $
 *                                                                           
 */

#include "SrvMsgLeaseQuery.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvIfaceMgr.h"
#include "SrvMsgAdvertise.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "AddrClient.h"

TSrvMsgLeaseQuery::TSrvMsgLeaseQuery(SmartPtr<TSrvIfaceMgr> IfaceMgr, 
                               //SmartPtr<TSrvTransMgr> TransMgr,
                               SmartPtr<TSrvCfgMgr>   ConfMgr, 
                               SmartPtr<TSrvAddrMgr>  AddrMgr, 
                               int iface,  SmartPtr<TIPv6Addr> addr,
                               char* buf,
                               int bufSize)
    :TSrvMsg(IfaceMgr,0,ConfMgr,AddrMgr,iface,addr,buf,bufSize) {
}

void TSrvMsgLeaseQuery::doDuties() {
    return;
}

bool TSrvMsgLeaseQuery::check() {
    // TODO: validation
    return true;
}

// unsigned long TSrvMsgLeaseQuery::getTimeout() {
//     return 0;
// }

TSrvMsgLeaseQuery::~TSrvMsgLeaseQuery() {

}

string TSrvMsgLeaseQuery::getName() {
    return "LEASE-QUERY";
}
