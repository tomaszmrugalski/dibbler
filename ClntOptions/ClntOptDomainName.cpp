/*                                                                           *
 * Dibbler - a portable DHCPv6                                               *
 *                                                                           *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>                           *
 *          Marek Senderski <msend@o2.pl>                                    *
 *                                                                           *
 * released under GNU GPL v2 or later licence                                */

#include "ClntOptDomainName.h"
#include "Portable.h"
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
    char buf[200];
    strncpy(buf,DomainName.c_str(),199);
    domain_add("",0, buf);
    return true;
}
//
//
//
//    TClntMsg* parent=(TClntMsg*)Parent;
//    SmartPtr<TClntCfgMgr> cfgMgr=parent->getClntCfgMgr();
//    SmartPtr<TClntCfgIface> ptrIface=cfgMgr->getIface(parent->getIface());
//    if (this->SrvDUID)
//        ptrIface->setDomainName(DomainName,SrvDUID);
//    else
//        clog<< logger::logCrit 
//            << "Trying to set new domain lis without setting server DUID"<<logger::endl;
//    return true;
//}

void TClntOptDomainName::setSrvDuid(SmartPtr<TDUID> duid)
{
    this->SrvDUID=duid;
}
