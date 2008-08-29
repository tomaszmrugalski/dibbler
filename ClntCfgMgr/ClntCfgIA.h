/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntCfgIA.h,v 1.9 2008-08-29 00:07:27 thomson Exp $
 */

class TClntCfgIA;
#ifndef CLNTCFGIA_H
#define CLNTCFGIA_H


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

    unsigned long getT1();
    unsigned long getT2();

    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);

    void firstAddr();
    SmartPtr<TClntCfgAddr> getAddr();
    long countAddr();
    void addAddr(SmartPtr<TClntCfgAddr> addr);

    TClntCfgIA();
    TClntCfgIA(SmartPtr<TClntCfgIA> right, long iAID);

    void setState(enum EState state);
    enum EState getState();
    bool getAddrParams();
    
  private:
    unsigned long IAID;
    unsigned long T1;
    unsigned long T2;
    
    EState State;	
    List(TClntCfgAddr) ClntCfgAddrLst;

    bool AddrParams;
};

#endif 

