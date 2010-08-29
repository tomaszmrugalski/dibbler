/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SmartPtr.h"
#include "ClntCfgMgr.h"
#include "ClntOptOptionRequest.h"

TClntOptOptionRequest::TClntOptOptionRequest(SPtr<TClntCfgIface> ptrIface, TMsg* parent)
	:TOptOptionRequest(parent) {
    // requested options are no longer added here
    // see void TClntMsg::appendRequestedOptions() for details
}

TClntOptOptionRequest::TClntOptOptionRequest( char * buf,  int n, TMsg* parent)
	:TOptOptionRequest(buf,n, parent) {
}

bool TClntOptOptionRequest::doDuties() {
    return false;
}
