/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include "SrvOptTimeZone.h"

TSrvOptTimeZone::TSrvOptTimeZone(string domain, TMsg* parent)
    :TOptTimeZone(domain, parent)
{

}

TSrvOptTimeZone::TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent)
    :TOptTimeZone(buf,bufsize, parent)
{

}

bool TSrvOptTimeZone::doDuties()
{
    return true;
}
