/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptLifetime.cpp,v 1.2 2006-10-29 13:11:47 thomson Exp $
 *
 */

#include "DHCPConst.h"
#include "SrvOptLifetime.h"

TSrvOptLifetime::TSrvOptLifetime(char * buf,  int n, TMsg* parent)
	:TOptInteger(OPTION_LIFETIME, OPTION_LIFETIME_LEN, buf,n, parent){

}

TSrvOptLifetime::TSrvOptLifetime( unsigned int lifetime, TMsg* parent)
	:TOptInteger(OPTION_LIFETIME, OPTION_LIFETIME_LEN, lifetime, parent) {
}

bool TSrvOptLifetime::doDuties()
{
    return false;
}
