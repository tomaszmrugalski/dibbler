/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptNISDomain.cpp,v 1.2 2008-08-29 00:07:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
 */

#include "SrvOptNISDomain.h"
#include "DHCPConst.h"

TSrvOptNISDomain::TSrvOptNISDomain(string fqdn, TMsg* parent)
    :TOptString(OPTION_NIS_DOMAIN_NAME, fqdn, parent) {

}

TSrvOptNISDomain::TSrvOptNISDomain(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_NIS_DOMAIN_NAME, buf, bufsize, parent) {

}

bool TSrvOptNISDomain::doDuties() {
    return true;
}
