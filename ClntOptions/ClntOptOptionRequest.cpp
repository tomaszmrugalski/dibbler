/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptOptionRequest.cpp,v 1.5 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2004-10-25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
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
