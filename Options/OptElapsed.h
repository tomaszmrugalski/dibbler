#ifndef OPTELAPSED_H
#define OPTELAPSED_H

#include "Opt.h"

class TOptElapsed : public TOpt
{
  public:
    TOptElapsed(TMsg* parent);
    TOptElapsed( char * &buf,  int &n, TMsg* parent);
    int getSize();
    char * storeSelf( char* buf);
protected:
    unsigned long Timestamp;
};

#endif /* OPTELAPSED_H */
