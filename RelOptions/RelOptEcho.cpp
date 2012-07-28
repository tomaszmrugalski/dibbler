/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SmartPtr.h"
#include "RelOptEcho.h"

TRelOptEcho::TRelOptEcho( char * buf,  int n, TMsg* parent)
    :TOptOptionRequest(OPTION_ERO, buf, n, parent)
{
}

TRelOptEcho::TRelOptEcho(TMsg* parent)
    :TOptOptionRequest(OPTION_ERO, parent)
{
}

bool TRelOptEcho::doDuties()
{
    return true;
}
