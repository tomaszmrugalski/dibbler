#ifndef USERCLASS_H_HEADER_INCLUDED_C11211AD
#define USERCLASS_H_HEADER_INCLUDED_C11211AD

#include "Opt.h"
#include "DHCPConst.h"


class TOptUserClass : public TOpt
{
  public:
    TOptUserClass( char * &buf,  int &n, TMsg* parent);

    
    //##Documentation
    //## Klasa reprezentuje ka¿d¹ wiadomoœæ DHCP.
     int getSize();
     bool isValid();
     char * storeSelf( char* buf);
};



#endif /* USERCLASS_H_HEADER_INCLUDED_C11211AD */
