/*
 * Dibbler - a portable DHCPv6
 *
 * author: Krzysztof Wnuk <keczi@poczta.onet.pl>
 *
 */

class TOptIA_PD;
#ifndef OPTIA_PD_H
#define OPTIA_PD_H

#include <stdint.h>
#include "Opt.h"

class TOptIA_PD : public TOpt
{
  public:
    TOptIA_PD(uint32_t iaid, uint32_t t1, uint32_t t2, TMsg* parent);
    TOptIA_PD(char * &buf, int &bufsize, TMsg* parent);
    size_t getSize();
    int getStatusCode();

    uint32_t getIAID() const;
    uint32_t getT1() const;
    uint32_t getT2() const;
    int countPrefixes();

    void setT1(uint32_t t1);
    void setT2(uint32_t t2);
    void setIAID(uint32_t iaid);

    char * storeSelf( char* buf);
    bool isValid() const;
 protected:
    uint32_t IAID_;
    uint32_t T1_;
    uint32_t T2_;
    bool Valid_;
};


#endif /*  */
