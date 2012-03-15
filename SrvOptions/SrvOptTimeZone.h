/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */


#ifndef SRVOPTTIMEZONE_H
#define SRVOPTTIMEZONE_H

#include "OptString.h"

class TSrvOptTimeZone : public TOptString
{
public:
    TSrvOptTimeZone(std::string timezone, TMsg* parent);
    TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
