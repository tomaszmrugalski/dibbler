#ifndef USERCLASS_H
#define USERCLASS_H

#include "Opt.h"
#include "DHCPConst.h"


class TOptUserClass : public TOpt
{
 public:
    TOptUserClass( char * &buf,  int &n, TMsg* parent);
    
    int getSize();
    bool isValid();
    char * storeSelf( char* buf);
};

#endif /* USERCLASS_H */
