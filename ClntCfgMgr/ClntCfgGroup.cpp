#include "ClntCfgGroup.h"
#include "SmartPtr.h"
#include <iostream>
#include <iomanip>
#include "Logger.h"

using namespace std;

TClntCfgGroup::TClntCfgGroup()
{
    this->isRapidcommit = false;
}

void TClntCfgGroup::firstIA()
{
    ClntCfgIALst.first();
}

SmartPtr<TClntCfgIA> TClntCfgGroup::getIA()
{
    return ClntCfgIALst.get();
}

void TClntCfgGroup::addIA(SmartPtr<TClntCfgIA> ptr)
{
    ClntCfgIALst.append(ptr);
}

long TClntCfgGroup::countIA()
{
    return ClntCfgIALst.count();
}

void TClntCfgGroup::setOptions(SmartPtr<TClntParsGlobalOpt> opt)
{
    SmartPtr<TStationID> Station;
   
    opt->firstPrefSrv();
    while(Station=opt->getPrefSrv())
        this->PrefSrv.append(Station);
    
    opt->firstRejedSrv();
    while(Station=opt->getRejedSrv())
	this->RejedSrv.append(Station);
    this->isRapidcommit=opt->getRapidCommit();
}

bool TClntCfgGroup::getRapidCommit()
{
    return this->isRapidcommit;
}

bool TClntCfgGroup::isServerRejected(SmartPtr<TIPv6Addr> addr,SmartPtr<TDUID> duid)
{
    this->RejedSrv.first();
    SmartPtr<TStationID> RejectedSrv;
    while(RejectedSrv=RejedSrv.get())
    {
        if (((*RejectedSrv)==addr)||((*RejectedSrv)==duid))
            return true;
    }
    return false;
}

ostream& operator<<(ostream& out,TClntCfgGroup& group)
{
    out << "      <group>" << logger::endl;
    out << "        <!-- prefered servers count="<<group.PrefSrv.count() << " -->" << std::endl;
    
    SmartPtr<TStationID> StationIDPtr;
    
    group.PrefSrv.first();
    while(StationIDPtr=group.PrefSrv.get())
	out<<*StationIDPtr << logger::endl;
    
    out << "        <!-- rejected servers count=" << group.RejedSrv.count() << " -->" << std::endl;
    group.RejedSrv.first();
    while(StationIDPtr=group.RejedSrv.get())
	out << *StationIDPtr << std::endl;
    
    // out << "      <-- IA count=" << group.ClntCfgIALst.count() << " -->" << std::endl;
    
    SmartPtr<TClntCfgIA> IA;
    group.ClntCfgIALst.first();
    while(IA=group.ClntCfgIALst.get())
	out << *IA;
    
    out << "      </group>" << std::endl;
    return out;
}
