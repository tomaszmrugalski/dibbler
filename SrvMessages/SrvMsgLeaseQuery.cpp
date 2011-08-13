/*
 * Dibbler - a portable DHCPv6
 *
 * author:  Tomasz Mrugalski <thomson(at)klub.com.pl>
 * changes: Karol Podolski <podol(at)ds.pg.gda.pl>
 *
 * released under GNU GPL v2 only licence
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
#include "Logger.h"

TSrvMsgLeaseQuery::TSrvMsgLeaseQuery(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize, bool istcp /* =false*/ )
    :TSrvMsg(iface,addr,buf,bufSize) {
    tcp = istcp;
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

string TSrvMsgLeaseQuery::getName() {
    return "LEASE-QUERY";
}
