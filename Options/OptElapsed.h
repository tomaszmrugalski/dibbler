#ifndef ELAPSED_H_HEADER_INCLUDED_C112034D
#define ELAPSED_H_HEADER_INCLUDED_C112034D

#include "Opt.h"

class TOptElapsed : public TOpt
{
  public:
    TOptElapsed(TMsg* parent);
    TOptElapsed( char * &buf,  int &n, TMsg* parent);
    int getSize();
    
    char * storeSelf( char* buf);

protected:
     long Timestamp;
};

#endif /* ELAPSED_H_HEADER_INCLUDED_C112034D */
