#ifndef CLNTOPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18
#define CLNTOPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18
#include "OptInterfaceID.h"

//##ModelId=3EFF07DE032C
class TClntOptInterfaceID : public TOptInterfaceID 
{
  public:
    TClntOptInterfaceID(char * buf,int n, TMsg* parent);
	bool doDuties();
};



#endif /* OPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18 */
