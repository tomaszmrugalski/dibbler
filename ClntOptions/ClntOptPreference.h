#ifndef CLNTPREFERENCE_H
#define CLNTPREFERENCE_H

#include "DHCPConst.h"
#include "OptPreference.h"

class TClntOptPreference : public TOptPreference 
{
  public:
    TClntOptPreference( char * buf,  int n, TMsg* parent);

    TClntOptPreference( char pref, TMsg* parent);
	bool doDuties();
};

#endif
