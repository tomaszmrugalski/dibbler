/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptLifetime.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "DHCPConst.h"
#include "SrvOptLifetime.h"

TSrvOptLifetime::TSrvOptLifetime(char * buf,  int n, TMsg* parent)
	:TOptInteger4(OPTION_LIFETIME, buf,n, parent){

}

TSrvOptLifetime::TSrvOptLifetime( unsigned int lifetime, TMsg* parent)
	:TOptInteger4(OPTION_LIFETIME, lifetime, parent) {
}

bool TSrvOptLifetime::doDuties()
{
    return false;
}
