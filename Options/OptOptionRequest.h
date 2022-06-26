/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTOPTIONREQUEST_H
#define OPTOPTIONREQUEST_H

#include "DHCPConst.h"
#include "Opt.h"
#include "SmartPtr.h"
class TClntConfMgr;

class TOptOptionRequest : public TOpt {
public:
  TOptOptionRequest(uint16_t code, TMsg *parent);
  TOptOptionRequest(uint16_t code, const char *buf, size_t size, TMsg *parent);

  void addOption(unsigned short optNr);
  void delOption(unsigned short optNr);
  bool isOption(unsigned short optNr);
  int count();
  void clearOptions();

  virtual size_t getSize();
  virtual char *storeSelf(char *buf);
  int getReqOpt(int optNr);
  virtual std::string getPlain();
  virtual bool doDuties() { return true; }
  virtual ~TOptOptionRequest();

protected:
  unsigned short
      *Options; /// @todo: you're kidding me, right? Rewrite this ASAP
  int OptCnt;
};

#endif
