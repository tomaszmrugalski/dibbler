/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#ifndef OPTNTPSERVERS_H
#define OPTNTPSERVERS_H
#include "IPv6Addr.h"
#include "Container.h"
#include "SmartPtr.h"
#include "Opt.h"

class TOptNTPServers : public TOpt
{

public:
    TOptNTPServers(TContainer<SmartPtr<TIPv6Addr> > NTPsrvLst, TMsg* parent);
    TOptNTPServers(char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    void firstNTPSrv();
    SmartPtr<TIPv6Addr> getNTPSrv();
    bool isValid();
protected:
    TContainer<SmartPtr<TIPv6Addr> > NTPSrv;
    bool Valid;
};

#endif
