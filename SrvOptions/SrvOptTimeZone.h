/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptTimeZone.h,v 1.3 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */


#ifndef SRVOPTTIMEZONE_H
#define SRVOPTTIMEZONE_H

#include "OptString.h"

class TSrvOptTimeZone : public TOptString
{
public:
    TSrvOptTimeZone(string timezone, TMsg* parent);
    TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
