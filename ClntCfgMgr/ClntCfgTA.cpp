/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntCfgTA.cpp,v 1.3 2007-01-07 20:18:44 thomson Exp $
 */

#include "ClntCfgTA.h"
#include <iostream>
#include <iomanip>
#include "Logger.h"
using namespace std;

TClntCfgTA::TClntCfgTA() {
    static unsigned long lastIAID = 1;
    this->iaid = lastIAID++;
    this->State = STATE_NOTCONFIGURED;
}

void TClntCfgTA::setState(enum EState state) {
    this->State = state;
}

enum EState TClntCfgTA::getState() {
    return this->State;
}

unsigned long TClntCfgTA::getIAID() {
    return this->iaid;
}

void TClntCfgTA::setIAID(unsigned long iaid) {
    this->iaid=iaid;
}

ostream& operator<<(ostream& out,TClntCfgTA& ta)
{
    out << "    <ta iaid=\"" << ta.iaid << "\" state=\"" << StateToString(ta.State) << "\" />" << std::endl;
    return out;
}
