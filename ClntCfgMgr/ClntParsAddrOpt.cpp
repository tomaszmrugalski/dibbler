/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * Released under GNU GPL v2 licence
 *
 * $Id: ClntParsAddrOpt.cpp,v 1.3 2004-10-25 20:45:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include <limits.h>
#include "ClntParsAddrOpt.h"

TClntParsAddrOpt::TClntParsAddrOpt()
{
    Pref=ULONG_MAX;
    Valid=ULONG_MAX;
}

long TClntParsAddrOpt::getPref()
{
    return Pref;
}

void TClntParsAddrOpt::setPref(long pref)
{
    this->Pref=pref;
}

long TClntParsAddrOpt::getValid()
{
    return Valid;
}

void TClntParsAddrOpt::setValid(long valid)
{
    Valid=valid;
}
