/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgDecline.cpp,v 1.2 2004-06-20 17:51:48 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "ClntMsgDecline.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "AddrIA.h"
#include "ClntOptIA_NA.h"

TClntMsgDecline::TClntMsgDecline(SmartPtr<TClntIfaceMgr> IfaceMgr,
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr> CfgMgr,
				 SmartPtr<TClntAddrMgr> AddrMgr,
				 int iface, SmartPtr<TIPv6Addr> addr,
				 TContainer< SmartPtr< TAddrIA> > IALst)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,DECLINE_MSG)
{
    IRT=DEC_TIMEOUT;
    MRT=0;
    //these both below mean there is no ending condition and transactions
    //lasts till receiving answer
    MRC=DEC_MAX_RC;
    MRD=0;
    RT=0;

    // include our ClientIdentifier
    SmartPtr<TOpt> ptr;
    ptr = new TClntOptClientIdentifier( CfgMgr->getDUID(), this );
    Options.append( ptr );
    
    // include server's DUID
    ptr = new TClntOptServerIdentifier(IALst.getFirst()->getDUID(),this);
    Options.append( ptr );
    
    // create IAs
    SmartPtr<TAddrIA> ptrIA;
    IALst.first();
    while ( ptrIA = IALst.get() ) {
	Options.append( new TClntOptIA_NA(ptrIA,this));
    }
    pkt = new char[getSize()];

}

TClntMsgDecline::TClntMsgDecline(SmartPtr<TClntIfaceMgr> IfaceMgr,
				 SmartPtr<TClntTransMgr> TransMgr,
				 SmartPtr<TClntCfgMgr> CfgMgr,
				 SmartPtr<TClntAddrMgr> AddrMgr,
				 int iface, SmartPtr<TIPv6Addr> addr, char* buf, int bufSize)
    :TClntMsg(IfaceMgr,TransMgr,CfgMgr,AddrMgr,iface,addr,buf,bufSize)
{
	pkt=NULL;
}

void TClntMsgDecline::answer(SmartPtr<TMsg> rep)
{
    //FIXME: Is UseMulticast option included?

    SmartPtr<TClntOptServerIdentifier> repSrvID= (Ptr*)  rep->getOption(OPTION_SERVERID);
    SmartPtr<TClntOptServerIdentifier> msgSrvID= (Ptr*)  this->getOption(OPTION_SERVERID);
    if ((!repSrvID)||
        (!(*msgSrvID->getDUID()==*repSrvID->getDUID())))
       return;
    IsDone = true;
}

void TClntMsgDecline::doDuties()
{
    if (RC!=MRC)
       	send();
    else
	IsDone=true;
    return;
}

bool TClntMsgDecline::check()
{
	return false;
}

string TClntMsgDecline::getName() {
    return "DECLINE";
}


TClntMsgDecline::~TClntMsgDecline()
{
}
