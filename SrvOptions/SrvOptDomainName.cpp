/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptDomainName.cpp,v 1.3 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptDomainName.h"
#include "DHCPConst.h"

TSrvOptDomainName::TSrvOptDomainName(List(string) domains, TMsg* parent)
    :TOptStringLst(OPTION_DOMAIN_LIST, domains, parent)
{
}

TSrvOptDomainName::TSrvOptDomainName(char *buf, int bufsize, TMsg* parent)
    :TOptStringLst(OPTION_DOMAIN_LIST, buf, bufsize, parent)
{

}

bool TSrvOptDomainName::doDuties()
{
    return true;
}
