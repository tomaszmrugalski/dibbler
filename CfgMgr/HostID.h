/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef STATIONID_H_
#define STATIONID_H_

#include "DUID.h"
#include "IPv6Addr.h"
#include "SmartPtr.h"

#include <iomanip>
#include <iostream>

class THostID {
  friend std::ostream &operator<<(std::ostream &out, THostID &station);

public:
  THostID(SPtr<TIPv6Addr> addr);
  THostID(SPtr<TDUID> duid);
  bool operator==(SPtr<TIPv6Addr> addr);
  bool operator==(SPtr<TDUID> duid);

  // THostID(const THostID& info);
  //~THostID();
private:
  bool isIDAddress;
  SPtr<TIPv6Addr> Addr;
  SPtr<TDUID> DUID;
};

#endif
