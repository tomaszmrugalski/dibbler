#ifndef CLNTCFGGROUP_H
#define CLNTCFGGROUP_H

#include "Container.h"
#include "SmartPtr.h"
#include "ClntCfgIA.h"
#include "StationID.h"
#include <iostream>
#include <iomanip>
using namespace std;

class TClntCfgGroup
{
 public:
    friend std::ostream& operator<<(std::ostream&,TClntCfgGroup&);
    TClntCfgGroup();
    
    // --- IA related methods ---
    void firstIA();
    SmartPtr<TClntCfgIA> getIA();
    void addIA(SmartPtr<TClntCfgIA> ptr);
    long countIA();
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);
    bool getRapidCommit();
    bool isServerRejected(SmartPtr<TIPv6Addr> addr,SmartPtr<TDUID> duid);
 private:
    bool isRapidcommit;
    TContainer< SmartPtr<TClntCfgIA> > ClntCfgIALst;
    TContainer< SmartPtr<TStationID> > PrefSrv;
    TContainer< SmartPtr<TStationID> > RejedSrv;
};

#endif
