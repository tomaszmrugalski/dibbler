#ifndef CLNTUSERCLASS_H_HEADER_INCLUDED_C11211AD
#define CLNTUSERCLASS_H_HEADER_INCLUDED_C11211AD

#include "OptUserClass.h"
#include "DHCPConst.h"

//##ModelId=3EC75CE30350
class TClntOptUserClass : public TOptUserClass 
{
  public:
    TClntOptUserClass( char * buf,  int n, TMsg* parent);

	bool doDuties();
};



#endif /* USERCLASS_H_HEADER_INCLUDED_C11211AD */
