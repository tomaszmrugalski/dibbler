#include "ClntCfgGroup.h"
#include "SmartPtr.h"
#include <iostream>
#include <iomanip>
#include "Logger.h"

using namespace std;

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
	out<<"Parametry Grupy:" << logger::endl;
    out<<"Liczba preferowanych serwerow:"<<group.PrefSrv.count() << logger::endl;
    
	SmartPtr<TStationID> StationIDPtr;
	
	group.PrefSrv.first();
	while(StationIDPtr=group.PrefSrv.get())
		out<<*StationIDPtr << logger::endl;

    out<<"Liczba odrzuconych serwerow:"<<group.RejedSrv.count() << logger::endl;
    group.RejedSrv.first();
	while(StationIDPtr=group.RejedSrv.get())
		out<<*StationIDPtr << logger::endl;

	out<<"Liczba IA:"<<group.ClntCfgIALst.count() << logger::endl;

	int IACnt=0;
	SmartPtr<TClntCfgIA> IA;
	group.ClntCfgIALst.first();
	while(IA=group.ClntCfgIALst.get())
	{	
		cout<<"Nr.IA:"<<IACnt++ << logger::endl;
		cout<<"{" << logger::endl<<*IA << logger::endl<<"}" << logger::endl;
	}
		

	return out;
}
