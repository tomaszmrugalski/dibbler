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