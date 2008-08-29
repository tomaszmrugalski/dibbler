/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "SrvOptUserClass.h"

TSrvOptUserClass::TSrvOptUserClass( char * buf,  int n, TMsg* parent)
	:TOptUserClass(buf,n, parent)
{

}
bool TSrvOptUserClass::doDuties()
{
    return true;
}
