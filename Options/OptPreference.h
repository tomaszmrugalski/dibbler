#ifndef PREFERENCE_H_HEADER_INCLUDED_C1126A41
#define PREFERENCE_H_HEADER_INCLUDED_C1126A41

#include "DHCPConst.h"
#include "Opt.h"

class TOptPreference : public TOpt
{
  public:
    TOptPreference( char * &buf,  int &n, TMsg* parent);

    TOptPreference( char pref, TMsg* parent);

     int getSize();
     char * storeSelf( char* buf);

  private:
     char Pref;

public:
     int getPreference(void);

};

#endif
