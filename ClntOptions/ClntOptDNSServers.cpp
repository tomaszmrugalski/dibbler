#include "ClntOptDNSServers.h"
#include "Logger.h"

TClntOptDNSServers::TClntOptDNSServers(TContainer<SmartPtr< TIPv6Addr> > lst, TMsg* parent)
    :TOptDNSServers(lst, parent)
{

}

TClntOptDNSServers::TClntOptDNSServers(char* buf, int size, TMsg* parent)
    :TOptDNSServers(buf,size, parent)
{

}

bool TClntOptDNSServers::doDuties()
{
    TClntMsg* parent=(TClntMsg*)Parent;
    SmartPtr<TClntCfgMgr> cfgMgr=parent->getClntCfgMgr();
    SmartPtr<TClntCfgIface> ptrIface=cfgMgr->getIface(parent->getIface());
    //FIXME:Here should be passed also server duid in order to renew parameter
    //      but how/maybe IP address will be enough 
    //      so duid server should be set before executing doDuties
    if (this->SrvDUID)
        ptrIface->setDNSSrv(DNSSrv,SrvDUID);
    else
        clog<< logger::logCrit 
            << "Trying to set new set of DNS servers without setting server DUID"<<logger::endl;
    return true;
}

void TClntOptDNSServers::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
