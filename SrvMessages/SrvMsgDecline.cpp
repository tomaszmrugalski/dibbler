/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 *                                                                           
 * released under GNU GPL v2 only licence                                
 *                                                                           
 */

#include "SrvMsgDecline.h"
#include "AddrClient.h"
#include "OptDUID.h"
    
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


std::string TSrvMsgDecline::getName() const {
    return "DECLINE";
}
