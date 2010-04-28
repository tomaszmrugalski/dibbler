/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntMsgDecline.cpp,v 1.8 2009-03-24 23:17:17 thomson Exp $
 *
 */

#include "ClntMsgDecline.h"
#include "ClntOptClientIdentifier.h"
#include "ClntOptServerIdentifier.h"
#include "AddrIA.h"
#include "ClntOptIA_NA.h"

TClntMsgDecline::TClntMsgDecline(int iface, SPtr<TIPv6Addr> addr,
                                 List(TAddrIA) IALst)
    :TClntMsg(iface, addr, DECLINE_MSG)
{
    IRT=DEC_TIMEOUT;
    MRT=0;
    //these both below mean there is no ending condition and transactions
    //lasts till receiving answer
    MRC=DEC_MAX_RC;
    MRD=0;
    RT=0;

    // include our ClientIdentifier
    SPtr<TOpt> ptr;
    ptr = new TClntOptClientIdentifier( ClntCfgMgr().getDUID(), this );
    Options.append( ptr );
    
    // include server's DUID
    ptr = new TClntOptServerIdentifier(IALst.getFirst()->getDUID(),this);
    Options.append( ptr );
    
    // create IAs
    SPtr<TAddrIA> ptrIA;
    IALst.first();
    while ( ptrIA = IALst.get() ) {
	Options.append( new TClntOptIA_NA(ptrIA,this));
    }

    appendElapsedOption();
    appendAuthenticationOption();
    this->IsDone = false;
    this->send();
    pkt = new char[getSize()];
    this->IsDone = false;
    // this->send();
}

void TClntMsgDecline::answer(SPtr<TClntMsg> rep)
{
    /// @todo: Is UseMulticast option included?

    SPtr<TClntOptServerIdentifier> repSrvID= (Ptr*)  rep->getOption(OPTION_SERVERID);
    SPtr<TClntOptServerIdentifier> msgSrvID= (Ptr*)  this->getOption(OPTION_SERVERID);
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
