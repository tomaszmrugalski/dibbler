/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include "SrvOptDomainName.h"

TSrvOptDomainName::TSrvOptDomainName(string domain, TMsg* parent)
    :TOptDomainName(domain, parent)
{
}

TSrvOptDomainName::TSrvOptDomainName(char *buf, int bufsize, TMsg* parent)
    :TOptDomainName(buf,bufsize, parent)
{

}

bool TSrvOptDomainName::doDuties()
{
    return true;
}
