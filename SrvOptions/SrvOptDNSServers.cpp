/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptDNSServers.cpp,v 1.5 2008-08-29 00:07:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2007-01-21 19:17:58  thomson
 * Option name constants updated (by Jyrki Soini)
 *
 * Revision 1.3  2004-10-25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include "SrvOptDNSServers.h"
#include "DHCPConst.h"

TSrvOptDNSServers::TSrvOptDNSServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_SERVERS, lst, parent) {

}

TSrvOptDNSServers::TSrvOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_SERVERS, buf,size, parent) {
    
}

bool TSrvOptDNSServers::doDuties() {    
    return true;
}
