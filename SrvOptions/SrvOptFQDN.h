/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptFQDN.h,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */


#ifndef SRVOPTFQDN_H
#define SRVOPTFQDN_H

#include "OptString.h"

class TSrvOptFQDN : public TOptString
{
public:
    TSrvOptFQDN(string fqdn, TMsg* parent);
    TSrvOptFQDN(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
