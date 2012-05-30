/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvMsgInfRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvMsgAdvertise.h"

TSrvMsgInfRequest::TSrvMsgInfRequest(int iface,  SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TSrvMsg(iface, addr, buf, bufSize) {
}

void TSrvMsgInfRequest::doDuties() {
    return;
}

bool TSrvMsgInfRequest::check() {
    /* client is not required (but is allowed) to include ClientID option, also ServerID is optional */
    return true;
}

unsigned long TSrvMsgInfRequest::getTimeout() {
    return 0;
}

std::string TSrvMsgInfRequest::getName() const {
    return "INF-REQUEST";
}


TSrvMsgInfRequest::~TSrvMsgInfRequest(){
}
