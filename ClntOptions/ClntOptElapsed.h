#ifndef CLNTELAPSED_H_HEADER_INCLUDED_C112034D
#define CLNTELAPSED_H_HEADER_INCLUDED_C112034D

#include "OptElapsed.h"

class TClntOptElapsed : public TOptElapsed
{
  public:
    TClntOptElapsed(TMsg* parent);
    TClntOptElapsed( char * buf,  int n, TMsg* parent);
    bool doDuties();
};
#endif
