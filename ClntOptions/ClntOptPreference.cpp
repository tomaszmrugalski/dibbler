/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptPreference.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "DHCPConst.h"
#include "ClntOptPreference.h"

TClntOptPreference::TClntOptPreference( char * buf,  int n, TMsg* parent)
	:TOptPreference(buf,n, parent)
{

}

TClntOptPreference::TClntOptPreference( char pref, TMsg* parent)
	:TOptPreference(pref, parent)
{
}

bool TClntOptPreference::doDuties()
{
    return false;
}
