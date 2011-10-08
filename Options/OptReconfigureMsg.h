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
    TOptReconfigureMsg(int type, int msgType, TMsg* parent);
    TOptReconfigureMsg(int type, char *&buf, int &bufsize, TMsg* parent);
    int getSize();

    char * storeSelf(char* buf);
    bool doDuties() { return true; }
    bool isValid();
  protected:
    int MSG_Type;
};

#endif
