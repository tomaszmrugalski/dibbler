class TOptIA_NA;
#ifndef OPTIA_NA_H
#define OPTIA_NA_H

#include "Opt.h"

class TOptIA_NA : public TOpt
{
  public:
    TOptIA_NA( long IAID, long t1,  long t2, TMsg* parent);
    TOptIA_NA( char * &buf, int &bufsize, TMsg* parent);
    int getSize();
    int getStatusCode();
    
    long getIAID();
    long getT1();
    long getT2();
    long getMaxValid();

    char * storeSelf( char* buf);
    bool isValid();
 protected:
     bool Valid;
     long IAID;
     long T1;
     long T2;
};



#endif /* IA_NA_H_HEADER_INCLUDED_C112064B */
