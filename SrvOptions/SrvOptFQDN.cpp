/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "SrvOptFQDN.h"

TSrvOptFQDN::TSrvOptFQDN(const std::string& fqdn, TMsg* parent)
    :TOptFQDN(fqdn, parent) {
	this->setNFlag(false);
}

TSrvOptFQDN::TSrvOptFQDN(char *buf, int bufsize, TMsg* parent)
    :TOptFQDN(buf, bufsize, parent) {

}

bool TSrvOptFQDN::doDuties() {
    return true;
}
