/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptNISServer.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptNISServer.h"
#include "DHCPConst.h"

TSrvOptNISServers::TSrvOptNISServers(List(TIPv6Addr) lst, TMsg* parent)
    :TOptAddrLst(OPTION_NIS_SERVERS, lst, parent) {

}

TSrvOptNISServers::TSrvOptNISServers(char* buf, int size, TMsg* parent)
    :TOptAddrLst(OPTION_NIS_SERVERS, buf,size, parent) {
    
}

bool TSrvOptNISServers::doDuties() {    
    return true;
}
