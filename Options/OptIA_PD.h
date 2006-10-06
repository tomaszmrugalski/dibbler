/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 * version 0.1  2006/09/08 23:53:54  thomson
 */

class TOptIA_PD;
#ifndef OPTIA_PD_H
#define OPTIA_PD_H

#include "Opt.h"

class TOptIA_PD : public TOpt
{
  public:
    TOptIA_PD( long IAID, long t1,  long t2, TMsg* parent);
    TOptIA_PD( char * &buf, int &bufsize, TMsg* parent);
    int getSize();
    int getStatusCode();
    
    unsigned long getIAID();
    unsigned long getT1();
    unsigned long getT2();
    unsigned long getMaxValid();
    int countPrefixes();
    
    char * storeSelf( char* buf);
    bool isValid();
 protected:
    bool Valid;
    unsigned long IAID;
    unsigned long T1;
    unsigned long T2;
};


#endif /*  */
