/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptNISPDomain.cpp,v 1.2 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
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
