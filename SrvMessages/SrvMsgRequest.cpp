/*                                                                           
 * Dibbler - a portable DHCPv6                                               
 *                                                                           
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           
 *          Marek Senderski <msend@o2.pl>                                    
 * changes: Krzysztof Wnuk <keczi@poczta.onet.pl>                                                                        
 * released under GNU GPL v2 only licence                                
 *                                                                           
 * $Id: SrvMsgRequest.cpp,v 1.9 2008-08-29 00:07:35 thomson Exp $
 *                                                                           
 */

#include "SrvMsgRequest.h"
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "SrvIfaceMgr.h"
#include "SrvMsgAdvertise.h"
#include "SrvOptServerIdentifier.h"
#include "SrvOptIA_NA.h"
#include "AddrClient.h"

TSrvMsgRequest::TSrvMsgRequest(int iface,  SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TSrvMsg(iface,addr,buf,bufSize) {
}

void TSrvMsgRequest::doDuties() {
    return;
}

bool TSrvMsgRequest::check() {
    return TSrvMsg::check(true /* ClientID required */, true /* ServerID required */);
}

unsigned long TSrvMsgRequest::getTimeout() {
    return 0;
}

TSrvMsgRequest::~TSrvMsgRequest() {

}

string TSrvMsgRequest::getName() {
    return "REQUEST";
}
