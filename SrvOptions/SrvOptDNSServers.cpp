/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptDNSServers.cpp,v 1.3 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptDNSServers.h"
#include "DHCPConst.h"

TSrvOptDNSServers::TSrvOptDNSServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_RESOLVERS, lst, parent) {

}

TSrvOptDNSServers::TSrvOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_DNS_RESOLVERS, buf,size, parent) {
    
}

bool TSrvOptDNSServers::doDuties() {    
    return true;
}
