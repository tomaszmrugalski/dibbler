#ifndef OPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18
#define OPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18
#include "Opt.h"

//##ModelId=3EFF07DE032C
class TOptInterfaceID : public TOpt
{
  public:
    TOptInterfaceID( char * &buf,  int &n, TMsg* parent);

    //##ModelId=3EFF083B03B9
     int getSize();

    //##ModelId=3EFF0840004E
     char * storeSelf( char *buf);
};



#endif /* OPTIONINTERFACEID_H_HEADER_INCLUDED_C100FE18 */
