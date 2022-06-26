/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#include "Opt.h"
#include <stdint.h>

#ifndef OPTIA_H
#define OPTIA_H

class TOptIA : public TOpt {
public:
  TOptIA(uint16_t type, uint32_t iaid, uint32_t t1, uint32_t t2, TMsg *parent);
  TOptIA(uint16_t type, TMsg *parent);

  unsigned long getIAID() const;
  unsigned long getT1() const;
  unsigned long getT2() const;
  void setT1(unsigned long t1);
  void setT2(unsigned long t2);
  void setIAID(uint32_t iaid);

  int getStatusCode();

protected:
  unsigned long IAID_;
  unsigned long T1_;
  unsigned long T2_;
  bool Valid_;
};

#endif
