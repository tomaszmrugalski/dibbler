/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptVendorClass.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "OptVendorClass.h"
TOptVendorClass::TOptVendorClass( char * &buf,  int &n, TMsg* parent)	:TOpt(OPTION_VENDOR_CLASS, parent){
}int TOptVendorClass::getSize(){	return 0;}



 char * TOptVendorClass::storeSelf( char* buf)
{
	return 0;
}

bool TOptVendorClass::isValid()
{
    return true;
}
