#ifndef CLNTCFGADDR_H_HEADER_INCLUDED_C0FDAB75
#define CLNTCFGADDR_H_HEADER_INCLUDED_C0FDAB75

#include "DHCPConst.h"
#include "ClntParsGlobalOpt.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include <iostream>
#include <iomanip>

using namespace std;

class TClntCfgAddr
{
	friend ostream& operator<<(ostream& out,TClntCfgAddr& group);
  public:
    SmartPtr<TIPv6Addr> get();

    long getValid();

    long getPref();
	
    TClntCfgAddr(SmartPtr<TIPv6Addr> addr,long valid,long pref);
    
    ~TClntCfgAddr();
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);

    TClntCfgAddr();

    TClntCfgAddr(SmartPtr<TIPv6Addr> addr);

  private:
    SmartPtr<TIPv6Addr> Addr;
    long Valid;
    long Pref;
	
    ESendOpt ValidSendOpt;
    ESendOpt PrefSendOpt;
};

#endif
