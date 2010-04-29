/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvMsgRenew.cpp,v 1.6 2008-08-29 00:07:35 thomson Exp $
 *
 */

#include "SrvMsgRenew.h"
#include "SrvOptClientIdentifier.h"
#include "SrvOptServerIdentifier.h"
#include "SrvCfgMgr.h"
#include "DHCPConst.h"

TSrvMsgRenew::TSrvMsgRenew(int iface,  SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TSrvMsg(iface,addr,buf,bufSize) {

}

void TSrvMsgRenew::doDuties() {
}

unsigned long TSrvMsgRenew::getTimeout() {
    return 0;
}

bool TSrvMsgRenew::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID required */);
}

string TSrvMsgRenew::getName() {
    return "RENEW";
}

TSrvMsgRenew::~TSrvMsgRenew() {
}
