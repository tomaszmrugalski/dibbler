/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptDNSServers.cpp,v 1.5 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */


#include "ClntOptDNSServers.h"
#include "Logger.h"

TClntOptDNSServers::TClntOptDNSServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent)
    :TOptDNSServers(lst, parent)
{

}

TClntOptDNSServers::TClntOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptDNSServers(buf,size, parent)
{

}

bool TClntOptDNSServers::doDuties()
{
    TClntMsg* parent=(TClntMsg*)Parent;

    SmartPtr<TIPv6Addr> ptrAddr;
    DNSSrv.first();

    while (ptrAddr = DNSSrv.get() ) {
	dns_add("",0,ptrAddr->getPlain() );
    }
	return true;
}

void TClntOptDNSServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
