/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTSTATUSCODE_H
#define OPTSTATUSCODE_H

#include "Opt.h"
#include <string>

class TOptStatusCode : public TOpt {
public:
  TOptStatusCode(const char *buf, size_t len, TMsg *parent);
  TOptStatusCode(int status, const std::string &Message, TMsg *parent);

  virtual size_t getSize();
  virtual char *storeSelf(char *buf);

  int getCode();
  std::string getText();

  virtual bool doDuties();

private:
  std::string Message_;
  int StatusCode_;
  bool Valid_;
};

#endif
