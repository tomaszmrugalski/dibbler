/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptVendorClass.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "ClntOptVendorClass.h"

TClntOptVendorClass::TClntOptVendorClass( char * buf,  int n, TMsg* parent)
	:TOptVendorClass(buf,n, parent)
{
	
}
bool TClntOptVendorClass::doDuties()
{
    return false;
}
