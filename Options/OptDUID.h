/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTDUID_H
#define OPTDUID_H

#include "DHCPConst.h"
#include "Opt.h"
#include "DUID.h" 
#include "SmartPtr.h"

class TOptDUID : public TOpt
{
  public:
    TOptDUID(int type, SmartPtr<TDUID> duid, TMsg* parent);
    TOptDUID(int type, char* &buf, int &bufsize, TMsg* parent);
    int getSize();

    char * storeSelf(char* buf);
    SmartPtr<TDUID> getDUID();
    bool isValid();
  protected:
    SmartPtr<TDUID> DUID;
};

#endif
