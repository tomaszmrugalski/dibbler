/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptTimeZone.cpp,v 1.5 2008-11-13 22:40:26 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2008-08-29 00:07:37  thomson
 * Temporary license change(GPLv2 or later -> GPLv2 only)
 *
 * Revision 1.3  2004-10-25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include "SrvOptTimeZone.h"
#include "DHCPConst.h"

TSrvOptTimeZone::TSrvOptTimeZone(string domain, TMsg* parent)
    :TOptString(OPTION_NEW_TZDB_TIMEZONE, domain, parent)
{

}

TSrvOptTimeZone::TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_NEW_TZDB_TIMEZONE, buf, bufsize, parent)
{

}

bool TSrvOptTimeZone::doDuties()
{
    return true;
}
