/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptServerUnicast.cpp,v 1.2 2004-09-05 15:27:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#include "SrvOptServerUnicast.h"

TSrvOptServerUnicast::TSrvOptServerUnicast( char * buf,  int n, TMsg* parent)
	:TOptServerUnicast(buf,n, parent)
{

}

TSrvOptServerUnicast::TSrvOptServerUnicast(SmartPtr<TIPv6Addr> addr, TMsg* parent) 
    :TOptServerUnicast(addr, parent) {

}

bool TSrvOptServerUnicast::doDuties()
{
    return true;
}
