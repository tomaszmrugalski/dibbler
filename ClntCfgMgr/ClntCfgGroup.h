#ifndef CLNTCFGGROUP_H_HEADER_INCLUDED_C0FDDA6F
#define CLNTCFGGROUP_H_HEADER_INCLUDED_C0FDDA6F

#include "Container.h"
#include "SmartPtr.h"
#include "ClntCfgIA.h"
#include "StationID.h"
#include <iostream>
#include <iomanip>
using namespace std;


//##ModelId=3EF9E8FF0336
class TClntCfgGroup
{
 public:
    friend ostream& operator<<(ostream&,TClntCfgGroup&);
    
    
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



#endif /* CLNTCFGGROUP_H_HEADER_INCLUDED_C0FDDA6F */
