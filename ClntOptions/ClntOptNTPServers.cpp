/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include "ClntOptNTPServers.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptNTPServers::TClntOptNTPServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent)
    :TOptNTPServers(lst, parent)
{
}

TClntOptNTPServers::TClntOptNTPServers(char* buf, int size, TMsg* parent)
    :TOptNTPServers(buf,size, parent)
{
    
}

bool TClntOptNTPServers::doDuties()
{
    TClntMsg* parent=(TClntMsg*)Parent;
    SmartPtr<TClntCfgMgr> cfgMgr=parent->getClntCfgMgr();
    SmartPtr<TClntCfgIface> ptrIface=cfgMgr->getIface(parent->getIface());
    //FIXME:Here should be passed also server duid in order to renew parameter
    //      but how/maybe IP address will be enough 
    //      so duid server should be set before executing doDuties
    if (this->SrvDUID)
        ptrIface->setNTPSrv(NTPSrv,SrvDUID);
    else
        clog<< logger::logCrit 
            << "Trying to set new set of NTP servers without setting server DUID"<<logger::endl;
    return true;
}

void TClntOptNTPServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
