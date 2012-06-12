/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvMsgLeaseQuery.cpp,v 1.4 2008-08-29 00:07:35 thomson Exp $
 *                                                                           
 */

#include "SrvMsgLeaseQuery.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvIfaceMgr.h"
#include "SrvMsgAdvertise.h"
#include "SrvOptIA_NA.h"
#include "AddrClient.h"
#include "Logger.h"

TSrvMsgLeaseQuery::TSrvMsgLeaseQuery(int iface,  SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TSrvMsg(iface,addr,buf,bufSize) {
}


void TSrvMsgLeaseQuery::doDuties() {
    return;
}

bool TSrvMsgLeaseQuery::check() {
    /// @todo: validation
    if (!getOption(OPTION_CLIENTID)) {
	Log(Warning) << "LQ: Lease Query message does not contain required CLIENT-ID option." << LogEnd;
	return false;
    }

    return true;
}

TSrvMsgLeaseQuery::~TSrvMsgLeaseQuery() {

}

std::string TSrvMsgLeaseQuery::getName() const {
    return "LEASE-QUERY";
}
