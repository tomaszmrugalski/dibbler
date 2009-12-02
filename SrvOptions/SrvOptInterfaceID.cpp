/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptInterfaceID.cpp,v 1.7 2009-03-09 22:45:58 thomson Exp $
 *
 */

#include "SrvOptInterfaceID.h"
#include "DHCPConst.h"
#include <string.h>

#ifdef WIN32
#include <WinSock2.h>
#else
#include <netinet/in.h>
#endif

/** 
 * compares two interface-ids
 * 
 * @param other 
 * 
 * @return true, if both interface-IDs are the same
 */
bool TSrvOptInterfaceID::operator==(const TSrvOptInterfaceID &other) const
{
    if (DataLen != other.DataLen)
	return false;
    if (!memcmp(Data, other.Data, DataLen))
	return true;
    return false;
}

TSrvOptInterfaceID::TSrvOptInterfaceID(int id, TMsg * parent)
    :TOptGeneric(OPTION_INTERFACE_ID, (char*)&id, sizeof(int), parent)
{
    int tmp = htonl(id);
    memmove(Data, &tmp, sizeof(int));
}

TSrvOptInterfaceID::TSrvOptInterfaceID( char * buf,  int n, TMsg* parent)
    :TOptGeneric(OPTION_INTERFACE_ID, buf,n, parent) {
}

bool TSrvOptInterfaceID::doDuties() {
    return true;
}
