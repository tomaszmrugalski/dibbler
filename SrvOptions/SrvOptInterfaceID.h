#ifndef SRVOPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18
#define SRVOPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18
#include "OptInterfaceID.h"

class TSrvOptInterfaceID : public TOptInterfaceID 
{
  public:
    TSrvOptInterfaceID( char * buf,  int n, TMsg* parent);
	bool doDuties();
};



#endif /* OPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18 */
