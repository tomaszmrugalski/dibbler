/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTADDRPARAMS_H
#define OPTADDRPARAMS_H

#include "OptInteger.h"

class TOptAddrParams : public TOptInteger {
public:
  TOptAddrParams(const char *buf, size_t len, TMsg *parent);

  int getPrefix();
  int getBitfield();
  bool doDuties();
};

#endif
