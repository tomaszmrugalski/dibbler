/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptNISPDomain.h,v 1.1 2004-11-02 01:30:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */


#ifndef SRVOPTNISPDOMAIN_H
#define SRVOPTNISPDOMAIN_H

#include "OptString.h"

class TSrvOptNISPDomain : public TOptString
{
public:
    TSrvOptNISPDomain(string domain, TMsg* parent);
    TSrvOptNISPDomain(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
