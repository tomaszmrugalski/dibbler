/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

class TOptIA_NA;
#ifndef OPTIA_NA_H
#define OPTIA_NA_H

#include "Opt.h"

class TOptIA_NA : public TOpt
{
  public:
    TOptIA_NA( long IAID, long t1,  long t2, TMsg* parent);
    TOptIA_NA( char * &buf, int &bufsize, TMsg* parent);
    size_t getSize();
    int getStatusCode();

    unsigned long getIAID();
    unsigned long getT1();
    unsigned long getT2();
    void setT1(unsigned long t1);
    void setT2(unsigned long t2);
    unsigned long getMaxValid();
    int countAddrs();

    char * storeSelf( char* buf);
    bool isValid();
 protected:
    unsigned long IAID_;
    unsigned long T1_;
    unsigned long T2_;
    bool Valid_;
};


#endif /*  */
