#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "SmartPtr.h"
#include "ClntCfgMgr.h"
#include "ClntOptOptionRequest.h"


TClntOptOptionRequest::TClntOptOptionRequest(SmartPtr<TClntCfgIface> ptrIface, TMsg* parent)
	:TOptOptionRequest(parent)
{
    //FIXME: Here should be read list of options from CfgMgr
    if (ptrIface->isReqDNSSrv()&&(ptrIface->getDNSSrvState()==NOTCONFIGURED))
        this->addOption(OPTION_DNS_RESOLVERS);
    if (ptrIface->isReqNTPSrv()&&(ptrIface->getNTPSrvState()==NOTCONFIGURED))
        this->addOption(OPTION_NTP_SERVERS);
    if (ptrIface->isReqDomainName()&&(ptrIface->getDomainNameState()==NOTCONFIGURED))
        this->addOption(OPTION_DOMAIN_LIST);
    if (ptrIface->isReqTimeZone()&&(ptrIface->getTimeZoneState()==NOTCONFIGURED))
        this->addOption(OPTION_TIME_ZONE);
}

TClntOptOptionRequest::TClntOptOptionRequest( char * buf,  int n, TMsg* parent)
	:TOptOptionRequest(buf,n, parent)
{
}

bool TClntOptOptionRequest::doDuties()
{
    return false;
}
 