/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski  <msend@o2.pl>                                   *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef SRVOPTTIMEZONE_H
#define SRVOPTTIMEZONE_H

#include "OptTimeZone.h"

class TSrvOptTimeZone : public TOptTimeZone
{
public:
    TSrvOptTimeZone(string domain, TMsg* parent);
    TSrvOptTimeZone(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
};
#endif
