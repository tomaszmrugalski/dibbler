/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptVendorSpecInfo.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#include "ClntOptVendorSpecInfo.h"

TClntOptVendorSpecInfo::TClntOptVendorSpecInfo( char * buf,  int n, TMsg* parent)
	:TOptVendorSpecInfo(buf,n, parent)
{

}
