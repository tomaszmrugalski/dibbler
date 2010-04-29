/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgConfirm.cpp,v 1.6 2008-08-29 00:07:34 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "AddrClient.h"
#include "SrvMsgConfirm.h"

TSrvMsgConfirm::TSrvMsgConfirm(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TSrvMsg(iface, addr,buf,bufSize) {
}

void TSrvMsgConfirm::doDuties() {
}

unsigned long TSrvMsgConfirm::getTimeout() {
    return 0;
}

bool TSrvMsgConfirm::check() {
    return TSrvMsg::check(true /* ClientID required */, false /* ServerID not allowed */);
}

TSrvMsgConfirm::~TSrvMsgConfirm() {
}

string TSrvMsgConfirm::getName() {
    return "CONFIRM";
}
