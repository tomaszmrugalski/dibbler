#include "ClntCfgIA.h"
#include <iostream>
#include <iomanip>
#include "Logger.h"
using namespace std;


long TClntCfgIA::countAddr()
{
    return ClntCfgAddrLst.count();
}

long TClntCfgIA::getT1()
{
    return T1;
}

long TClntCfgIA::getT2()
{
    return T2;
}

void TClntCfgIA::setState(enum EState state)
{
    State = state;
}

enum EState TClntCfgIA::getState()
{
    return State;
}

long  TClntCfgIA::getIAID()
{
    return IAID;
}

void TClntCfgIA::setIAID(long iaid)
{
    IAID=iaid;
}

void TClntCfgIA::setOptions(SmartPtr<TClntParsGlobalOpt> opt)
{
    this->T1=opt->getT1();
    this->T1SendOpt=opt->getT1SendOpt();
    this->T2=opt->getT2();
    this->T2SendOpt=opt->getT2SendOpt();
}

void TClntCfgIA::firstAddr()
{
    ClntCfgAddrLst.first();
}

SmartPtr<TClntCfgAddr> TClntCfgIA::getAddr()
{
    return ClntCfgAddrLst.get();
}


TClntCfgIA::TClntCfgIA(long iAID)
{
    IAID=iAID;
    T1=0;
    T2=0;
    T1SendOpt=Send;
    T2SendOpt=Send;
}

TClntCfgIA::TClntCfgIA(SmartPtr<TClntCfgIA> right, long iAID)
    :ClntCfgAddrLst(right->ClntCfgAddrLst)
{
    IAID=iAID;
    T1=right->getT1();
    T2=right->getT2();
    T1SendOpt=right->getT1SendOpt();
    T2SendOpt=right->getT2SendOpt();
}
ESendOpt TClntCfgIA::getT1SendOpt()
{
    return this->T1SendOpt;
}

ESendOpt TClntCfgIA::getT2SendOpt()
{
    return this->T2SendOpt;
}

void TClntCfgIA::addAddr(SmartPtr<TClntCfgAddr> addr)
{
    this->ClntCfgAddrLst.append(addr);    
}

ostream& operator<<(ostream& out,TClntCfgIA& ia)
{
    out<<"Parametry IA:" << logger::endl;
    out<<"T1:"<<ia.T1 << logger::endl;
    out<<"Send opt. T1:"<<ia.T1SendOpt << logger::endl;
    out<<"T2:"<<ia.T2 << logger::endl;
    out<<"Send opt. T2:"<<ia.T2SendOpt << logger::endl;

    out<<"Liczba Adres�w:"<<ia.ClntCfgAddrLst.count() << logger::endl;
    int addrCnt=0;
    SmartPtr<TClntCfgAddr> addr;
    ia.ClntCfgAddrLst.first();
    while(addr=ia.ClntCfgAddrLst.get())
    {	
        cout<<"Nr.addr:"<<addrCnt++ << logger::endl;
        cout<<"{" << logger::endl<<*addr << logger::endl<<"}" << logger::endl;
    }
    return out;
}
