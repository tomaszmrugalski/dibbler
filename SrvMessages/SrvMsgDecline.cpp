/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvMsgDecline.cpp,v 1.6 2008-08-29 00:07:34 thomson Exp $
 *                                                                           
 */

#include "SrvMsgDecline.h"
#include "AddrClient.h"
#include "SrvOptServerIdentifier.h"
    
TSrvMsgDecline::TSrvMsgDecline(int iface, SPtr<TIPv6Addr> addr,
			       char* buf, int bufSize)
    :TSrvMsg(iface,addr,buf,bufSize) {
}

void TSrvMsgDecline::doDuties() {
}

unsigned long TSrvMsgDecline::getTimeout() {
    return 0;
}

bool TSrvMsgDecline::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID mandatory */);
}

TSrvMsgDecline::~TSrvMsgDecline() {
}


string TSrvMsgDecline::getName() {
    return "DECLINE";
}
