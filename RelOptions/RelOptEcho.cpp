/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelOptEcho.cpp,v 1.1 2008-03-02 20:01:42 thomson Exp $
 *
 */

#include "SmartPtr.h"
#include "SrvCfgMgr.h"
#include "RelOptEcho.h"

TRelOptEcho::TRelOptEcho( char * buf,  int n, TMsg* parent)
	:TOptOptionRequest(buf,n, parent)
{
    OptType = OPTION_ERO;
}

TRelOptEcho::TRelOptEcho(TMsg* parent)
    :TOptOptionRequest(parent)
{
    OptType = OPTION_ERO;
}

bool TRelOptEcho::doDuties()
{
    return true;
}
