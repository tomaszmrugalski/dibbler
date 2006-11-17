/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptAuthentication.cpp,v 1.1 2006-11-17 00:39:04 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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

/* [s] - tu trzeba dodaæ parametry opcji AUTH */
//TSrvOptAuthentication::TSrvOptAuthentication(int status,string message, TMsg* parent)
  TSrvOptAuthentication::TSrvOptAuthentication(TMsg* parent)
	:TOptAuthentication(parent) {
}

bool TSrvOptAuthentication::doDuties()
{
    return false;
}
