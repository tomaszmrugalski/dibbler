/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgAdvertise.cpp,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "ClntMsgAdvertise.h"
#include "OptPreference.h"
#include "ClntOptClientIdentifier.h"

TClntMsgAdvertise::TClntMsgAdvertise(SmartPtr<TClntIfaceMgr> IfaceMgr,
				     SmartPtr<TClntTransMgr> TransMgr,
				     SmartPtr<TClntCfgMgr> CfgMgr,
				     SmartPtr<TClntAddrMgr> AddrMgr,
				     int iface, SmartPtr<TIPv6Addr> addr)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,ADVERTISE_MSG)
{
    pkt=NULL;
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
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,buflen)
{
    pkt=NULL;
}

bool TClntMsgAdvertise::check()
{
    //Clients MUST discard any received Advertise messages that meet any of
    //the following conditions:

    //-  the message does not include a Server Identifier option.
    if (!this->getOption(OPTION_SERVERID))
        return false;
    //-  the message does not include a Client Identifier option.
    SmartPtr<TClntOptClientIdentifier> clnID;
    if (!(clnID=(Ptr*)this->getOption(OPTION_CLIENTID)))
        return false;
    //-  the contents of the Client Identifier option does not match the
    //   client's DUID.
    if (!(*(clnID->getDUID())==(*(this->ClntCfgMgr->getDUID())) ))
        return false;
    //-  the "transaction-id" field value does not match the value the
    //   client used in its Solicit me - it was checked outside this function
    return true;
}

int TClntMsgAdvertise::getPreference()
{
    // FIXME: here we can implement more sophisticated pointing scheme, not just 
    //        pure PREFERENCE OPTION value. Number of SUCCESS IAs for example 
    //        could be taken into account
    SmartPtr<TOptPreference> ptr;
    ptr = (Ptr*) this->getOption(OPTION_PREFERENCE);
    if (!ptr)
        return 0;
    return ptr->getPreference();
}

void TClntMsgAdvertise::answer(SmartPtr<TMsg> Rep)
{
    // this should never happen
}

void TClntMsgAdvertise::doDuties()
{
    // this should never happen
}

string TClntMsgAdvertise::getName() {
    return "ADVERTISE";
}

TClntMsgAdvertise::~TClntMsgAdvertise()
{
}

