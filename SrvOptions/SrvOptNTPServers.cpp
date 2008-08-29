/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptNTPServers.cpp,v 1.5 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2007-01-21 19:17:58  thomson
 * Option name constants updated (by Jyrki Soini)
 *
 * Revision 1.3  2004-10-25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "SrvOptNTPServers.h"
#include "DHCPConst.h"

TSrvOptNTPServers::TSrvOptNTPServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_SNTP_SERVERS, lst, parent) {

}

TSrvOptNTPServers::TSrvOptNTPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_SNTP_SERVERS, buf, size, parent) {
    
}

bool TSrvOptNTPServers::doDuties()
{
    return true;
}
