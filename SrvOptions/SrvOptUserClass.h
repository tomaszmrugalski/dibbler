#ifndef SRVUSERCLASS_H_HEADER_INCLUDED_C11211AD
#define SRVUSERCLASS_H_HEADER_INCLUDED_C11211AD

#include "OptUserClass.h"
#include "DHCPConst.h"

class TSrvOptUserClass : public TOptUserClass 
{
 public:
    TSrvOptUserClass( char * buf,  int n, TMsg* parent);
    bool doDuties();
};



#endif 
