/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvOptTimeZone.h"
#include "DHCPConst.h"

TSrvOptTimeZone::TSrvOptTimeZone(std::string domain, TMsg* parent)
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
