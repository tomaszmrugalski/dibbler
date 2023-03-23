/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TClntCfgIA;
#ifndef CLNTCFGIA_H
#define CLNTCFGIA_H

#include "ClntCfgAddr.h"
#include "ClntParsGlobalOpt.h"
#include "DHCPConst.h"
#include <iomanip>
#include <iostream>

class TClntCfgIA {
  friend std::ostream &operator<<(std::ostream &out, TClntCfgIA &group);

public:
  long getIAID();
  void setIAID(long iaid);

  unsigned long getT1();
  unsigned long getT2();

  void setOptions(SPtr<TClntParsGlobalOpt> opt);

  void firstAddr();
  SPtr<TClntCfgAddr> getAddr();
  long countAddr();
  void addAddr(SPtr<TClntCfgAddr> addr);

  TClntCfgIA();
  TClntCfgIA(SPtr<TClntCfgIA> right, long iAID);

  void reset();
  void setState(enum EState state);
  enum EState getState();
  bool getAddrParams();

private:
  unsigned long IAID;
  unsigned long T1;
  unsigned long T2;
  EState State;
  List(TClntCfgAddr) ClntCfgAddrLst;
  static long newID();

  bool AddrParams; /// experimental address parameters feature
};

#endif
