/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntCfgAddr.h,v 1.3 2004-10-25 20:45:52 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 */

#ifndef CLNTCFGADDR_H
#define CLNTCFGADDR_H

#include "DHCPConst.h"
#include "ClntParsGlobalOpt.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"
#include <iostream>
#include <iomanip>

using namespace std;

class TClntCfgAddr
{
	friend std::ostream& operator<<(std::ostream& out,TClntCfgAddr& group);
  public:
    SmartPtr<TIPv6Addr> get();

    unsigned long getValid();

    unsigned long getPref();
	
    TClntCfgAddr(SmartPtr<TIPv6Addr> addr,long valid,long pref);
    
    ~TClntCfgAddr();
    void setOptions(SmartPtr<TClntParsGlobalOpt> opt);

    TClntCfgAddr();

    TClntCfgAddr(SmartPtr<TIPv6Addr> addr);

  private:
    SmartPtr<TIPv6Addr> Addr;
    unsigned long Valid;
    unsigned long Pref;
};

#endif
