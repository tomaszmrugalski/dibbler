/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptEcho.cpp,v 1.2 2008-08-29 00:07:36 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "SrvCfgMgr.h"
#include "SrvOptEcho.h"

TSrvOptEcho::TSrvOptEcho( char * buf,  int n, TMsg* parent)
	:TOptOptionRequest(buf,n, parent)
{
    OptType = OPTION_ERO;
}

TSrvOptEcho::TSrvOptEcho(TMsg* parent)
    :TOptOptionRequest(parent)
{

}

bool TSrvOptEcho::doDuties()
{
    return true;
}
