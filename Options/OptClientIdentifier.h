#ifndef CLIENTIDENTIFIER_H
#define CLIENTIDENTIFIER_H

#include "Opt.h"
#include "SmartPtr.h"
#include "DUID.h"
class TOptClientIdentifier : public TOpt
{
  public:
    TOptClientIdentifier(SmartPtr<TDUID> duid, TMsg* parent);
    TOptClientIdentifier(char* &buf, int &n, TMsg* parent);
    
    int getSize();

    char * storeSelf(char* buf);
	
    //int getDUIDlen();
    
    SmartPtr<TDUID> getDUID();
    bool isValid();
  private:
    SmartPtr<TDUID> DUID;
     //int DUIDlen;
    //char* DUID;
};

#endif
