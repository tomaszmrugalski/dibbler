/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptInterfaceID.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "OptInterfaceID.h"
#include "DHCPConst.h"

TOptInterfaceID::TOptInterfaceID( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_INTERFACE_ID, parent)
{

}

//##ModelId=3EFF083B03B9
 int TOptInterfaceID::getSize()
{
	return 0;
}

//##ModelId=3EFF0840004E
 char * TOptInterfaceID::storeSelf( char *)
{
    // FIXME
    return NULL;
}
