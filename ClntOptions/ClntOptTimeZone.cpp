/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptTimeZone.cpp,v 1.4 2004-03-29 19:10:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "ClntOptTimeZone.h"
#include "ClntMsg.h"
#include "Logger.h"

TClntOptTimeZone::TClntOptTimeZone(string domain, TMsg* parent)    :TOptTimeZone(domain, parent){
}
TClntOptTimeZone::TClntOptTimeZone(char *buf, int bufsize, TMsg* parent)    :TOptTimeZone(buf,bufsize, parent){
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
