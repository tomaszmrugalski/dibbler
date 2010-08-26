/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgReply.cpp,v 1.9 2008-08-29 00:07:28 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "ClntMsgReply.h"
#include "ClntOptIAAddress.h"

TClntMsgReply::TClntMsgReply(int iface, SPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TClntMsg(iface, addr,buf,bufSize)
{
}

void TClntMsgReply::answer(SPtr<TClntMsg> Reply) {
    // this should never happen. After receiving REPLY for e.g. REQUEST,
    // request->answer(reply) is called. Client nevers sends reply msg, so
    // this method will never be called.
}

void TClntMsgReply::doDuties() {
}

bool TClntMsgReply::check() {
    bool anonInfReq = ClntCfgMgr().anonInfRequest();
    return TClntMsg::check(!anonInfReq /* clientID mandatory */, true /* serverID mandatory */ );
}

string TClntMsgReply::getName() {
    return "REPLY";
}

SPtr<TIPv6Addr> TClntMsgReply::getFirstAddr() {
    firstOption();
    SPtr<TOpt> opt, subopt;
    while (opt=getOption()) {
	if (opt->getOptType() != OPTION_IA_NA)
	    continue;
	opt->firstOption();
	while (subopt=opt->getOption()) {
	    if (subopt->getOptType() != OPTION_IAADDR)
		continue;
	    SPtr<TOptIAAddress> optAddr = (Ptr*) subopt;
	    return optAddr->getAddr();
	}
    }

    return 0;
}

TClntMsgReply::~TClntMsgReply() {
}
