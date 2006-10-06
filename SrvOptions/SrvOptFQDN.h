/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * 
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptFQDN.h,v 1.3 2006-10-06 00:37:59 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2006-03-03 20:49:54  thomson
 * FQDN support improved.
 *
 * Revision 1.1  2004/11/02 01:30:54  thomson
 * Initial version.
 *
 */

#ifndef SRVOPTFQDN_H
#define SRVOPTFQDN_H

#include "OptFQDN.h"

class TSrvOptFQDN : public TOptFQDN
{
public:
    TSrvOptFQDN(string fqdn, TMsg* parent);
    TSrvOptFQDN(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif
