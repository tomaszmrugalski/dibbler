#ifndef CLNTCFGIA_H_HEADER_INCLUDED_C0FDC1EF
#define CLNTCFGIA_H_HEADER_INCLUDED_C0FDC1EF


#include "ClntCfgAddr.h"
#include "ClntParsGlobalOpt.h"
#include "DHCPConst.h"
#include <iostream>
#include <iomanip>
using namespace std;


class TClntCfgIA
{
	friend std::ostream& operator<<(std::ostream& out,TClntCfgIA& group);
 public:
    long getIAID();
    void setIAID(long iaid);

    long getT1();
    ESendOpt getT1SendOpt();

    long getT2();
    ESendOpt getT2SendOpt();

    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);

    void firstAddr();
    SmartPtr<TClntCfgAddr> getAddr();
    long countAddr();
    void addAddr(SmartPtr<TClntCfgAddr> addr);

    TClntCfgIA(long IAID);
    TClntCfgIA(SmartPtr<TClntCfgIA> right, long iAID);

    void setState(enum EState state);
    enum EState getState();

  private:
    long IAID;
    
    long T1;
    ESendOpt T1SendOpt;
    
    unsigned long T2;
    ESendOpt T2SendOpt;
    
    EState State;	
    TContainer< SmartPtr<TClntCfgAddr> > ClntCfgAddrLst;
};

#endif 
