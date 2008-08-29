/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: RelOptEcho.cpp,v 1.3 2008-08-29 00:07:32 thomson Exp $
 *
 */

#include "SmartPtr.h"
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
