/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntMsgRelease.cpp,v 1.4 2005-01-08 16:52:03 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/07/05 23:04:07  thomson
 * *** empty log message ***
 *
 * Revision 1.2  2004/06/20 17:51:48  thomson
 * getName() method implemented, comment cleanup
 *
 *
 */

#include "ClntMsgRelease.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "AddrIA.h"
#include "ClntOptServerIdentifier.h"
#include "ClntOptClientIdentifier.h"
#include "ClntCfgMgr.h"
#include "ClntOptIA_NA.h"
#include <cmath>
#include "Logger.h"
#include "ClntAddrMgr.h"
#include "AddrMgr.h"
#include "AddrIA.h"
#include "AddrAddr.h"
//Creates new tranasaction Release-Reply
//Input:
//  iaLst - list of TAddrIA, which concern one server and one link and in 
//          which all adddresses are not used and are removed from Address
//          Manager
TClntMsgRelease::TClntMsgRelease(
	SmartPtr<TClntIfaceMgr> IfMgr, 
	SmartPtr<TClntTransMgr> TransMgr,
	SmartPtr<TClntCfgMgr>   CfgMgr, 
	SmartPtr<TClntAddrMgr>  AddrMgr, 
	int iface, SmartPtr<TIPv6Addr> addr,
    TContainer<SmartPtr <TAddrIA> > iaLst)
	:TClntMsg(IfMgr, TransMgr, CfgMgr, AddrMgr, iface, addr, RELEASE_MSG)
{
    IRT=REL_TIMEOUT;
    MRT=0;
    MRC=REL_MAX_RC;
    MRD=0;
    RT=0;

    SmartPtr<TAddrIA> ia=iaLst.getFirst();
    Options.append(new TClntOptServerIdentifier(
                        ia->getDUID(),this));
    Options.append(new TClntOptClientIdentifier( CfgMgr->getDUID(),this));
    iaLst.first();
    while(ia=iaLst.get()) {
        Options.append(new TClntOptIA_NA(ia,this));
	SmartPtr<TAddrAddr> ptrAddr;
	SmartPtr<TIfaceIface> ptrIface;
	ptrIface = IfMgr->getIfaceByID(ia->getIface());
	ia->firstAddr();
	while (ptrAddr = ia->getAddr()) {
	    ptrIface->delAddr(ptrAddr->get());
	    Log(Notice)<< ptrAddr->get()->getPlain() 
		       << " address released from " << ptrIface->getName() << "/" 
		       << ptrIface->getID() << " interface." << LogEnd;
	}
    }

    pkt = new char[getSize()];
    IsDone = false;
}

void TClntMsgRelease::answer(SmartPtr<TClntMsg> rep)
{
    SmartPtr<TClntOptServerIdentifier> repSrvID= (Ptr*)  rep->getOption(OPTION_SERVERID);
    SmartPtr<TClntOptServerIdentifier> msgSrvID= (Ptr*)  this->getOption(OPTION_SERVERID);
    if ((!repSrvID)||
        (!(*msgSrvID->getDUID()==*repSrvID->getDUID())))
       return;
    IsDone=true;   
}

void TClntMsgRelease::doDuties()
{
    if (RC!=MRC)
       	send();
    else
       IsDone=true;
    return;
}

bool TClntMsgRelease::check()
{
	return false;
}

string TClntMsgRelease::getName() {
    return "RELEASE";
}

TClntMsgRelease::~TClntMsgRelease()
{
}
