#ifndef CLNTOPTELAPSED_H
#define CLNTOPTELAPSED_H

#include "OptInteger4.h"

class TClntOptElapsed : public TOptInteger4
{
  public:
    TClntOptElapsed(TMsg* parent);
    TClntOptElapsed( char * buf,  int n, TMsg* parent);
    bool doDuties();
};
#endif
