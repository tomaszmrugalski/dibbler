/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 * 
 * released under GNU GPL v2 only licence
 */

#include "ClntCfgPD.h"
#include <iostream>
#include <iomanip>
#include "Logger.h"
using namespace std;

TClntCfgPD::TClntCfgPD() {
    static unsigned long nextIAID=1;
    this->IAID = nextIAID++;
    this->T1 = CLIENT_DEFAULT_T1;
    this->T2 = CLIENT_DEFAULT_T2;
    this->prefixLength = 64; // default value;
    this->State = STATE_NOTCONFIGURED;
}

long TClntCfgPD::countPrefixes()
{
    return ClntCfgPrefixLst.count();
}

unsigned long TClntCfgPD::getT1() {
    return T1;
}

char TClntCfgPD::getPrefixLength() {
    return prefixLength;
}
unsigned long TClntCfgPD::getT2() {
    return T2;
}

void TClntCfgPD::setState(enum EState state) {
    State = state;
}

enum EState TClntCfgPD::getState() {
    return State;
}

long  TClntCfgPD::getIAID() {
    return IAID;
}

void TClntCfgPD::setIAID(long iaid) {
    IAID=iaid;
}

void TClntCfgPD::setOptions(SPtr<TClntParsGlobalOpt> opt) {
    this->T1=opt->getT1();
    this->T2=opt->getT2();
}

void TClntCfgPD::firstPrefix()
{
    ClntCfgPrefixLst.first();
}

SPtr<TClntCfgPrefix> TClntCfgPD::getPrefix() {
    return ClntCfgPrefixLst.get();
}

TClntCfgPD::TClntCfgPD(SPtr<TClntCfgPD> right, long iAID)
    :ClntCfgPrefixLst(right->ClntCfgPrefixLst)
{
    IAID=iAID;
    T1=right->getT1();
    T2=right->getT2();
}

void TClntCfgPD::addPrefix(SPtr<TClntCfgPrefix> prefix)
{
    this->ClntCfgPrefixLst.append(prefix);    
}

ostream& operator<<(ostream& out,TClntCfgPD& pd)
{
    out << "        <pd iaid=\"" << pd.IAID << "\" state=\"" << StateToString(pd.State) << "\" t1=\"" 
	<< pd.T1 << "\" t2=\"" << pd.T2 << "\" prefixes=\"" << pd.ClntCfgPrefixLst.count() << "\">" << std::endl;

    SPtr<TClntCfgPrefix> prefix;
    
    pd.ClntCfgPrefixLst.first();
    while(prefix=pd.ClntCfgPrefixLst.get())
    {	
        out << "          " << *prefix;
    }
    out << "        </pd>" << std::endl;
    return out;
}
