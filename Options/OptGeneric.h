/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptGeneric.h,v 1.3 2008-11-11 22:41:48 thomson Exp $
 *
 */

#ifndef OPTGENERIC_H
#define OPTGENERIC_H

#include "DHCPConst.h"
#include "DUID.h"
#include "Opt.h"
#include "SmartPtr.h"

class TOptGeneric : public TOpt {
public:
  bool operator==(const TOptGeneric &other);

  TOptGeneric(int optType, const char *data, unsigned short dataLen,
              TMsg *parent);
  TOptGeneric(int optType, TMsg *parent);
  ~TOptGeneric();
  size_t getSize();
  void setData(char *data, int dataLen);
  std::string getPlain();
  bool doDuties() { return true; }

  char *storeSelf(char *buf);
  virtual bool isValid() const;

protected:
  char *Data;
  int DataLen;
};

#endif
