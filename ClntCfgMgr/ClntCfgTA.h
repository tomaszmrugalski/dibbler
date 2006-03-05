/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntCfgTA.h,v 1.2 2006-03-05 21:38:47 thomson Exp $
 */

#ifndef CLNTCFGTA_H
#define CLNTCFGTA_H


#include "ClntParsGlobalOpt.h"
#include "DHCPConst.h"
#include <iostream>
#include <iomanip>
using namespace std;

class TClntCfgTA
{
    friend ostream& operator<<(ostream& out,TClntCfgTA& ta);
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

/*
 * $Log: not supported by cvs2svn $
 * Revision 1.1.2.1  2006/02/05 23:40:13  thomson
 * Initial revision.
 *
 */
