#include "ClntOptDomainName.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptDomainName::TClntOptDomainName(string domain, TMsg* parent)
    :TOptDomainName(domain, parent)
{
}

TClntOptDomainName::TClntOptDomainName(char *buf, int bufsize, TMsg* parent)
    :TOptDomainName(buf,bufsize, parent)
{
}

bool TClntOptDomainName::doDuties()
{
    TClntMsg* parent=(TClntMsg*)Parent;
    SmartPtr<TClntCfgMgr> cfgMgr=parent->getClntCfgMgr();
    SmartPtr<TClntCfgIface> ptrIface=cfgMgr->getIface(parent->getIface());
    //FIXME:Here should be passed also server duid in order to renew parameter
    //      but how/maybe IP address will be enough 
    //      so duid server should be set before executing doDuties
    if (this->SrvDUID)
        ptrIface->setDomainName(DomainName,SrvDUID);
    else
        clog<< logger::logCrit 
            << "Trying to set new domain lis without setting server DUID"<<logger::endl;
    return true;
}

void TClntOptDomainName::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
