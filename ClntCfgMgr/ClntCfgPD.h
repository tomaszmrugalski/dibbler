/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef CLNTCFGPD_H
#define CLNTCFGPD_H

#include "ClntCfgPrefix.h"
#include "ClntParsGlobalOpt.h"
#include "DHCPConst.h"
#include <iostream>
#include <iomanip>

class TClntCfgPD
{
    friend std::ostream& operator<<(std::ostream& out, TClntCfgPD& group);
 public:
    long getIAID();
    void setIAID(long iaid);

    unsigned long getT1();
    unsigned long getT2();
    char getPrefixLength();

    void setOptions(SPtr<TClntParsGlobalOpt> opt);

    void firstPrefix();
    SPtr<TClntCfgPrefix> getPrefix();
    long countPrefixes();
    void addPrefix(SPtr<TClntCfgPrefix> addr);

    TClntCfgPD();
    TClntCfgPD(SPtr<TClntCfgPD> right, long iAID);

    void setState(enum EState state);
    enum EState getState();

  private:
    unsigned long IAID;
    unsigned long T1;
    unsigned long T2;
    char prefixLength;

    EState State;
    List(TClntCfgPrefix) ClntCfgPrefixLst;
};

#endif
