/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptSIPServer.cpp,v 1.3 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2007-01-21 19:17:58  thomson
 * Option name constants updated (by Jyrki Soini)
 *
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
 */

#include "SrvOptSIPServer.h"
#include "DHCPConst.h"

TSrvOptSIPServers::TSrvOptSIPServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVER_A, lst, parent)
{

}

TSrvOptSIPServers::TSrvOptSIPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_SIP_SERVER_A, buf,size, parent)
{
    
}

bool TSrvOptSIPServers::doDuties()
{    
    return true;
}
