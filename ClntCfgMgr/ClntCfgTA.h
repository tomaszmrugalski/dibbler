/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#ifndef CLNTCFGTA_H
#define CLNTCFGTA_H

#include "ClntParsGlobalOpt.h"
#include "DHCPConst.h"
#include <iostream>
#include <iomanip>

class TClntCfgTA
{
    friend std::ostream& operator<<(std::ostream& out,TClntCfgTA& ta);
 public:
    TClntCfgTA();
    unsigned long getIAID();
    void setIAID(unsigned long iaid);
    void setState(enum EState state);
    enum EState getState();

  private:
    unsigned long iaid;
    EState State;	
};

#endif 
