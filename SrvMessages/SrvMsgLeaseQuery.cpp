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
#include "SrvOptIA_NA.h"
#include "AddrClient.h"
#include "Logger.h"

TSrvMsgLeaseQuery::TSrvMsgLeaseQuery(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize, int msgType, bool istcp /* =false*/ )
    :TSrvMsg(iface,addr, buf,bufSize, msgType) {
    //int iface, SPtr<TIPv6Addr> addr, char *buf,int bufSize, int msgType
    tcp = istcp;
}

void TSrvMsgLeaseQuery::doDuties() {
    return;
}

bool TSrvMsgLeaseQuery::check() {
    /// @todo: validation
    Log(Debug) << "Bulk Msg validation calling" << LogEnd;
    if (!getOption(OPTION_CLIENTID)) {
	Log(Warning) << "LQ: Lease Query message does not contain required CLIENT-ID option." << LogEnd;
	return false;
    }

    return true;
}

unsigned int TSrvMsgLeaseQuery::getBulkSize(char *buf)
{
    unsigned int bulkMsgSize;
    bulkMsgSize = readUint16(buf);
    return bulkMsgSize;
}

TSrvMsgLeaseQuery::~TSrvMsgLeaseQuery() {

}

std::string TSrvMsgLeaseQuery::getName() const {
    return "LEASE-QUERY";
}
bool TSrvMsgLeaseQuery::isTCP()
{
    return tcp;
}
