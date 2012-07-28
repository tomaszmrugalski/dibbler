/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptAuthentication.cpp,v 1.3 2008-08-29 00:07:35 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2008-02-25 17:49:12  thomson
 * Authentication added. Megapatch by Michal Kowalczuk.
 * (small changes by Tomasz Mrugalski)
 *
 * Revision 1.1  2006-11-17 00:39:04  thomson
 * Partial AUTH support by Sammael, fixes by thomson
 *
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include "SrvOptAuthentication.h"

TSrvOptAuthentication::TSrvOptAuthentication(char * buf,  int n, TMsg* parent)
	:TOptAuthentication(buf, n, parent) {
}

TSrvOptAuthentication::TSrvOptAuthentication(TMsg* parent)
	:TOptAuthentication(parent) {
    setRDM(0);
}

bool TSrvOptAuthentication::doDuties()
{
    return false;
}
