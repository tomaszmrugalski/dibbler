#include "ClntOptTimeZone.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptTimeZone::TClntOptTimeZone(string domain, TMsg* parent)
    :TOptTimeZone(domain, parent)
{

}

TClntOptTimeZone::TClntOptTimeZone(char *buf, int bufsize, TMsg* parent)
    :TOptTimeZone(buf,bufsize, parent)
{

}
bool TClntOptTimeZone::doDuties()
{
    TClntMsg* parent=(TClntMsg*)Parent;
    SmartPtr<TClntCfgMgr> cfgMgr=parent->getClntCfgMgr();
    SmartPtr<TClntCfgIface> ptrIface=cfgMgr->getIface(parent->getIface());
    //FIXME:Here should be passed also server duid in order to renew parameter
    //      but how/maybe IP address will be enough 
    //      so duid server should be set before executing doDuties
    
    if (this->SrvDUID)
        ptrIface->setTimeZone(Zone.get(),SrvDUID);
    else
        clog<< logger::logCrit 
            << "Trying to set new timezone lis without setting server DUID"<<logger::endl;
    return true;
}

void TClntOptTimeZone::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
