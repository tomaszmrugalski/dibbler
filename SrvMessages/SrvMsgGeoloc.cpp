/* 
 * File:   SrvMsgGeoloc.cpp
 * Author: Michal Golon
 * 
 */

#include "Portable.h"
#include "SrvMsgGeoloc.h"
#include "Msg.h"
#include "SmartPtr.h"
#include "SrvMsg.h"
#include "AddrClient.h"
#include "Logger.h"
#include <cmath>

TSrvMsgGeoloc::TSrvMsgGeoloc(int iface, SPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(iface,addr,buf,bufSize) {
}

void TSrvMsgGeoloc::doDuties() {
}

string TSrvMsgGeoloc::getName() {
    return "GEOLOC";
}

bool TSrvMsgGeoloc::check() {
    return TSrvMsg::check(true /* ClientID required */, false /* ServerID not allowed */);
}

unsigned long TSrvMsgGeoloc::getTimeout() {
	return 0;
}

TSrvMsgGeoloc::~TSrvMsgGeoloc() {
}

