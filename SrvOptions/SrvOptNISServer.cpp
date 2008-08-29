/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptNISServer.cpp,v 1.2 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
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
