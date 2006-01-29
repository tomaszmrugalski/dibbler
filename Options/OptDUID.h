#ifndef OPTDUID_H
#define OPTDUID_H

#include "DHCPConst.h"
#include "Opt.h"
#include "DUID.h" 
#include "SmartPtr.h"

class TOptDUID : public TOpt
{
  public:
    TOptDUID(SmartPtr<TDUID> duid, TMsg* parent);
    TOptDUID(char* &buf, int &bufsize, TMsg* parent);
    int getSize();

    char * storeSelf(char* buf);
    SmartPtr<TDUID> getDUID();
    bool isValid();
  protected:
    SmartPtr<TDUID> DUID;
};

#endif
