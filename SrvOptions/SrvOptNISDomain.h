/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptNISDomain.h,v 1.2 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:30:54  thomson
 * Initial version.
 *
 */


#ifndef SRVOPTNISDOMAIN_H
#define SRVOPTNISDOMAIN_H

#include "OptString.h"

class TSrvOptNISDomain : public TOptString
{
public:
    TSrvOptNISDomain(string fqdn, TMsg* parent);
    TSrvOptNISDomain(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
