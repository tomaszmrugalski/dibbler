/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvMsgRelease.cpp,v 1.4 2005-01-08 16:52:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/06/20 21:00:45  thomson
 * Various fixes.
 *
 * Revision 1.2  2004/06/20 17:25:07  thomson
 * getName() method implemented, clean up
 *
 *
 */
#include "SrvMsgRelease.h"
#include "OptServerIdentifier.h"
#include "AddrClient.h"

TSrvMsgRelease::TSrvMsgRelease(
	SmartPtr<TSrvIfaceMgr> IfMgr, 
	SmartPtr<TSrvTransMgr> TransMgr,
	SmartPtr<TSrvCfgMgr>   ConfMgr, 
	SmartPtr<TSrvAddrMgr>  AddrMgr, 
	int iface, 
	SmartPtr<TIPv6Addr> addr,
	char* buf,
	int bufSize)
    :TSrvMsg(IfMgr, TransMgr, ConfMgr, AddrMgr, iface, addr,buf,bufSize)
{
}

void TSrvMsgRelease::doDuties()
{
    //On server side nothing to do
    //there shouldn't be such a transaction
}

unsigned long TSrvMsgRelease::getTimeout()
{
    //On server side nothing to do
    //there shouldn't be such a transaction
    return 0x7fffffff;
}

bool TSrvMsgRelease::check()
{
    //Servers MUST discard any received Release message that	meets any of
    //the following conditions:
    //-	the	message	does not include a Client Identifier option.
    if(!getOption(OPTION_CLIENTID))
        return false;
    SmartPtr<TOptServerIdentifier> opt;
    //-	the	message	does not include a Server Identifier option.
    if (!(opt=(Ptr*)getOption(OPTION_SERVERID)))
        return false;
    //-	the	contents of	the	Server Identifier option does not match	the
    //	server's identifier.
    else
        return *(opt->getDUID())==*(SrvCfgMgr->getDUID());
}

string TSrvMsgRelease::getName() {
    return "RELEASE";
}

TSrvMsgRelease::~TSrvMsgRelease()
{
}
