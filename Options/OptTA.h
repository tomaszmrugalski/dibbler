/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TOptTA;
#ifndef OPTIA_TA_H
#define OPTIA_TA_H

#include "Opt.h"

// length without optType and Length
#define OPTION_IA_TA_LEN 4

class TOptTA : public TOpt {
public:
  TOptTA(uint32_t iaid, TMsg *parent);
  TOptTA(char *&buf, int &bufsize, TMsg *parent);
  size_t getSize();
  int getStatusCode();

  unsigned long getIAID();
  unsigned long getMaxValid();
  int countAddrs();

  char *storeSelf(char *buf);
  virtual bool isValid() const;
  bool doDuties() { return true; }

protected:
  uint32_t IAID_;
  bool Valid_;
};

#endif /* OPTIA_TA_H */
