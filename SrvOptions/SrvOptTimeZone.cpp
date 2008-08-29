/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptTimeZone.cpp,v 1.4 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004-10-25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include "SrvOptTimeZone.h"
#include "DHCPConst.h"

TSrvOptTimeZone::TSrvOptTimeZone(string domain, TMsg* parent)
    :TOptString(OPTION_TIME_ZONE, domain, parent)
{

}

TSrvOptTimeZone::TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_TIME_ZONE, buf, bufsize, parent)
{

}

bool TSrvOptTimeZone::doDuties()
{
    return true;
}
