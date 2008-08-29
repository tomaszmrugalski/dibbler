/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "SrvOptRapidCommit.h"

TSrvOptRapidCommit::TSrvOptRapidCommit( char * buf,  int n, TMsg* parent)
	:TOptRapidCommit(buf,n, parent)
{

}
TSrvOptRapidCommit::TSrvOptRapidCommit(TMsg* parent)
	:TOptRapidCommit(parent)
{
}

bool TSrvOptRapidCommit::doDuties()
{
    return true;
}
