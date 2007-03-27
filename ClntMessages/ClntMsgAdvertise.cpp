/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgAdvertise.cpp,v 1.5 2007-03-27 23:44:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2006-11-11 06:56:26  thomson
 * Improved required/not allowed options checking.
 *
 * Revision 1.3  2005-01-08 16:52:03  thomson
 * Relay support implemented.
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

#include "ClntMsgAdvertise.h"
#include "OptPreference.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptPreference.h"

TClntMsgAdvertise::TClntMsgAdvertise(SmartPtr<TClntIfaceMgr> IfaceMgr,
				     SmartPtr<TClntTransMgr> TransMgr,
				     SmartPtr<TClntCfgMgr> CfgMgr,
				     SmartPtr<TClntAddrMgr> AddrMgr,
				     int iface, SmartPtr<TIPv6Addr> addr)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,ADVERTISE_MSG) {
}

/* 
 * creates buffer based on buffer
 */
TClntMsgAdvertise::TClntMsgAdvertise(SmartPtr<TClntIfaceMgr> IfaceMgr,
				     SmartPtr<TClntTransMgr> TransMgr,
				     SmartPtr<TClntCfgMgr> CfgMgr,
				     SmartPtr<TClntAddrMgr> AddrMgr,
				     int iface, SmartPtr<TIPv6Addr> addr, 
				     char* buf, int buflen)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,buflen) {
}

bool TClntMsgAdvertise::check() {
    return TClntMsg::check(true /* clientID mandatory */, true /* serverID mandatory */ );
}

int TClntMsgAdvertise::getPreference() {
    SmartPtr<TOptPreference> ptr;
    ptr = (Ptr*) this->getOption(OPTION_PREFERENCE);
    if (!ptr)
        return 0;
    return ptr->getPreference();
}

void TClntMsgAdvertise::answer(SmartPtr<TClntMsg> Rep) {
    // this should never happen
}

void TClntMsgAdvertise::doDuties() {
    // this should never happen
}

string TClntMsgAdvertise::getName() {
    return "ADVERTISE";
}

string TClntMsgAdvertise::getInfo()
{
    ostringstream tmp;
    SPtr<TClntOptPreference> pref;
    SPtr<TClntOptServerIdentifier> srvID;

    pref  = (Ptr*) getOption(OPTION_PREFERENCE);
    srvID = (Ptr*) getOption(OPTION_SERVERID);

    if (srvID) {
	tmp << "Server ID=" << srvID->getDUID()->getPlain();
    } else {
	tmp << "malformed (Server ID option missing)";
    }

    if (pref) {
	tmp << ", preference=" << pref->getPreference();
    } else {
	tmp << ", no preference option, assumed 0";
    }
    
    return tmp.str();
}

TClntMsgAdvertise::~TClntMsgAdvertise() {
}

