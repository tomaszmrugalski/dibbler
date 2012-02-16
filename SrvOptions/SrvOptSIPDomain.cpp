/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "SrvOptSIPDomain.h"
#include "DHCPConst.h"

TSrvOptSIPDomain::TSrvOptSIPDomain(List(string) domains, TMsg* parent)    
    :TOptDomainLst(OPTION_SIP_SERVER_D, domains, parent) {
}

TSrvOptSIPDomain::TSrvOptSIPDomain(char *buf, int bufsize, TMsg* parent)
    :TOptDomainLst(OPTION_SIP_SERVER_D, buf,bufsize, parent) {
}

bool TSrvOptSIPDomain::doDuties() {
    return true;
}
