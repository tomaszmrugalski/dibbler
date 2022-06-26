/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TOptIA_NA;
#ifndef OPTIA_NA_H
#define OPTIA_NA_H

#include "OptIA.h"

class TOptIA_NA : public TOptIA {
public:
  TOptIA_NA(long IAID, long t1, long t2, TMsg *parent);
  TOptIA_NA(char *&buf, int &bufsize, TMsg *parent);
  size_t getSize();

  unsigned long getMaxValid();
  int countAddrs();

  char *storeSelf(char *buf);
  bool isValid() const;
};

#endif /*  */
