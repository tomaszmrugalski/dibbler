/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef CLNTOPTTIMEZONE_H
#define CLNTOPTTIMEZONE_H

#include "OptTimeZone.h"
#include "DUID.h"
#include "SmartPtr.h"

class TClntOptTimeZone : public TOptTimeZone
{
public:
    TClntOptTimeZone(string domain, TMsg* parent);
    TClntOptTimeZone(char *buf, int bufsize, TMsg* parent);
    bool doDuties();
    void setSrvDuid(SmartPtr<TDUID> duid);
private:
    SmartPtr<TDUID> SrvDUID;
};
#endif
