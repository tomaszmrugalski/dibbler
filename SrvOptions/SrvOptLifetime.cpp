/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptLifetime.cpp,v 1.4 2008-08-29 00:07:36 thomson Exp $
 *
 */

#include "DHCPConst.h"
#include "SrvOptLifetime.h"

TSrvOptLifetime::TSrvOptLifetime(char * buf,  int n, TMsg* parent)
	:TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, buf,n, parent){

}

TSrvOptLifetime::TSrvOptLifetime( unsigned int lifetime, TMsg* parent)
	:TOptInteger(OPTION_INFORMATION_REFRESH_TIME, OPTION_INFORMATION_REFRESH_TIME_LEN, lifetime, parent) {
}

bool TSrvOptLifetime::doDuties()
{
    return false;
}
