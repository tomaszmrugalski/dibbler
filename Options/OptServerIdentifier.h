#ifndef OPTSERVERIDENTIFIER_H
#define OPTSERVERIDENTIFIER_H

#include "DHCPConst.h"
#include "Opt.h"
#include "DUID.h" 
#include "SmartPtr.h"

class TOptServerIdentifier : public TOpt
{
  public:
    TOptServerIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
    TOptServerIdentifier(char* &buf, int &bufsize, TMsg* parent);
    int getSize();

    char * storeSelf(char* buf);
    //int getDUIDlen();
    SmartPtr<TDUID> getDUID();
    bool isValid();
  protected:
    SmartPtr<TDUID> DUID;
};

#endif
