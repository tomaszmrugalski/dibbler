/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptNISPDomain.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptNISPDomain.h"
#include "DHCPConst.h"

TSrvOptNISPDomain::TSrvOptNISPDomain(string domain, TMsg* parent)
    :TOptString(OPTION_NISP_DOMAIN_NAME, domain, parent) {

}

TSrvOptNISPDomain::TSrvOptNISPDomain(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_NISP_DOMAIN_NAME, buf, bufsize, parent) {

}

bool TSrvOptNISPDomain::doDuties() {
    return true;
}
