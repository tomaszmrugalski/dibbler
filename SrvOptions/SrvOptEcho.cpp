/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptEcho.cpp,v 1.1 2008-03-02 19:54:07 thomson Exp $
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
