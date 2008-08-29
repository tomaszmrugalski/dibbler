/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptSIPDomain.cpp,v 1.3 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2007-01-21 19:17:58  thomson
 * Option name constants updated (by Jyrki Soini)
 *
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
 * Revision 1.6  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 * Revision 1.5  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "SrvOptSIPDomain.h"
#include "DHCPConst.h"

TSrvOptSIPDomain::TSrvOptSIPDomain(List(string) domains, TMsg* parent)    
    :TOptStringLst(OPTION_SIP_SERVER_D, domains, parent) {
}

TSrvOptSIPDomain::TSrvOptSIPDomain(char *buf, int bufsize, TMsg* parent)
    :TOptStringLst(OPTION_SIP_SERVER_D, buf,bufsize, parent) {
}

bool TSrvOptSIPDomain::doDuties() {
    return true;
}
