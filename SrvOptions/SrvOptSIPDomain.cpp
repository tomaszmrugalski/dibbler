/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptSIPDomain.cpp,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.5  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "SrvOptSIPDomain.h"
#include "DHCPConst.h"

TSrvOptSIPDomain::TSrvOptSIPDomain(List(string) domains, TMsg* parent)    
    :TOptStringLst(OPTION_SIP_DOMAINS, domains, parent) {
}

TSrvOptSIPDomain::TSrvOptSIPDomain(char *buf, int bufsize, TMsg* parent)
    :TOptStringLst(OPTION_SIP_DOMAINS, buf,bufsize, parent) {
}

bool TSrvOptSIPDomain::doDuties() {
    return true;
}
