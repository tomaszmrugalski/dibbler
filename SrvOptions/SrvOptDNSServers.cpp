/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptDNSServers.cpp,v 1.4 2007-01-21 19:17:58 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
