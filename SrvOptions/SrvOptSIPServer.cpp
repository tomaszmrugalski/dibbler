/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptSIPServer.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptSIPServer.h"
#include "DHCPConst.h"

TSrvOptSIPServers::TSrvOptSIPServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVERS, lst, parent)
{

}

TSrvOptSIPServers::TSrvOptSIPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVERS, buf,size, parent)
{
    
}

bool TSrvOptSIPServers::doDuties()
{    
    return true;
}
