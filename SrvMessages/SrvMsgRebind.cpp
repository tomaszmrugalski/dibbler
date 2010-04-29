/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgRebind.cpp,v 1.6 2008-08-29 00:07:35 thomson Exp $
 *
 */
#include "SmartPtr.h"
#include "SrvMsg.h"
#include "SrvMsgRebind.h"
#include "AddrClient.h"

TSrvMsgRebind::TSrvMsgRebind(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TSrvMsg(iface, addr,buf,bufSize) {
}

void TSrvMsgRebind::doDuties() {
}

unsigned long TSrvMsgRebind::getTimeout() {
    return 0;
}

bool TSrvMsgRebind::check() {
    return TSrvMsg::check(true /* ClientID required */, false /* ServerID not allowed */);
}

string TSrvMsgRebind::getName() {
    return "REBIND";
}

TSrvMsgRebind::~TSrvMsgRebind() {
}
