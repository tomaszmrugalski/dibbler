/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptFQDN.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#include "SrvOptFQDN.h"
#include "DHCPConst.h"

TSrvOptFQDN::TSrvOptFQDN(string fqdn, TMsg* parent)
    :TOptString(OPTION_FQDN, fqdn, parent) {

}

TSrvOptFQDN::TSrvOptFQDN(char *buf, int bufsize, TMsg* parent)
    :TOptString(OPTION_FQDN, buf, bufsize, parent) {

}

bool TSrvOptFQDN::doDuties() {
    return true;
}
