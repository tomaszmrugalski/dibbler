/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include <iostream>
#include <iomanip>
#include "ClntCfgPD.h"
#include "DHCPDefaults.h"
#include "Logger.h"

using namespace std;

TClntCfgPD::TClntCfgPD()
    :T1_(CLIENT_DEFAULT_T1), T2_(CLIENT_DEFAULT_T2),
     PrefixLength_(64), State_(STATE_NOTCONFIGURED)
{
    static unsigned long nextIAID=1;
    IAID_ = nextIAID++;
}

long TClntCfgPD::countPrefixes() {
    return ClntCfgPrefixLst_.count();
}

unsigned long TClntCfgPD::getT1() {
    return T1_;
}

char TClntCfgPD::getPrefixLength() {
    return PrefixLength_;
}
unsigned long TClntCfgPD::getT2() {
    return T2_;
}

void TClntCfgPD::setState(enum EState state) {
    State_ = state;
}

enum EState TClntCfgPD::getState() {
    return State_;
}

long  TClntCfgPD::getIAID() {
    return IAID_;
}

void TClntCfgPD::setIAID(long iaid) {
    IAID_ = iaid;
}

void TClntCfgPD::setOptions(SPtr<TClntParsGlobalOpt> opt) {
    T1_ = opt->getT1();
    T2_ = opt->getT2();
}

void TClntCfgPD::firstPrefix() {
    ClntCfgPrefixLst_.first();
}

SPtr<TClntCfgPrefix> TClntCfgPD::getPrefix() {
    return ClntCfgPrefixLst_.get();
}

TClntCfgPD::TClntCfgPD(SPtr<TClntCfgPD> right, long iaid)
    :ClntCfgPrefixLst_(right->ClntCfgPrefixLst_), IAID_(iaid), T1_(right->getT1()),
     T2_(right->getT2()), PrefixLength_(right->getPrefixLength()) {
}

void TClntCfgPD::addPrefix(SPtr<TClntCfgPrefix> prefix) {
    ClntCfgPrefixLst_.append(prefix);
}

ostream& operator<<(ostream& out,TClntCfgPD& pd)
{
    out << "        <pd iaid=\"" << pd.IAID_ << "\" state=\"" << StateToString(pd.State_) << "\" t1=\""
        << pd.T1_ << "\" t2=\"" << pd.T2_ << "\" prefixes=\"" << pd.ClntCfgPrefixLst_.count() << "\">" << std::endl;

    SPtr<TClntCfgPrefix> prefix;

    pd.ClntCfgPrefixLst_.first();
    while(prefix = pd.ClntCfgPrefixLst_.get())
    {
        out << "          " << *prefix;
    }
    out << "        </pd>" << std::endl;
    return out;
}
