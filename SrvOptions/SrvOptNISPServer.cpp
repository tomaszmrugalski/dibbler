/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptNISPServer.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptNISPServer.h"
#include "DHCPConst.h"

TSrvOptNISPServers::TSrvOptNISPServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_NISP_SERVERS, lst, parent) {

}

TSrvOptNISPServers::TSrvOptNISPServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_NISP_SERVERS, buf,size, parent) {
    
}

bool TSrvOptNISPServers::doDuties() {    
    return true;
}
