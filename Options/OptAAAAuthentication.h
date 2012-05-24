/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTAAAAUTHENTICATION_H
#define OPTAAAAUTHENTICATION_H

#include "DHCPConst.h"
#include "Opt.h"
#include "Portable.h"

class TOptAAAAuthentication : public TOpt
{
public:
    TOptAAAAuthentication(char * &buf,  int &n,TMsg* parent);
    TOptAAAAuthentication(TMsg* parent);
    void setAAASPI(uint32_t value);
    uint32_t getAAASPI();
    size_t getSize();
    char * storeSelf(char* buf);

private:
    uint32_t AAASPI_;
};

#endif 
