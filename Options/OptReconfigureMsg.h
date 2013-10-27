/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Grzegorz Pluto
 * changes: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTRECONFIGURE_H
#define OPTRECONFIGURE_H

#include "DHCPConst.h"
#include "Opt.h"

class TOptReconfigureMsg : public TOpt
{
  public:
    TOptReconfigureMsg(int msgType, TMsg* parent);
    TOptReconfigureMsg(char *buf, int bufsize, TMsg* parent);
    size_t getSize();

    char* storeSelf(char* buf);
    bool doDuties() { return true; }
    bool isValid() const;
  protected:
    uint8_t MsgType_;
};

#endif
