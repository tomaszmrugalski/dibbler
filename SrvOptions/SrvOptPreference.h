#ifndef SRVPREFERENCE_H_HEADER_INCLUDED_C1126A41
#define SRVPREFERENCE_H_HEADER_INCLUDED_C1126A41

#include "DHCPConst.h"
#include "OptPreference.h"

class TSrvOptPreference : public TOptPreference 
{
  public:
    TSrvOptPreference( char * buf,  int n, TMsg* parent);
    TSrvOptPreference( char pref, TMsg* parent);

    bool doDuties();
};



#endif /* PREFERENCE_H_HEADER_INCLUDED_C1126A41 */
