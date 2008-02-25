/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: OptKeyGeneration.h,v 1.1 2008-02-25 20:42:46 thomson Exp $
 *
 */

#ifndef OPTKEYGENERATION_H
#define OPTKEYGENERATION_H

#include "DHCPConst.h"
#include "Opt.h"
#include "Portable.h"

class TOptKeyGeneration : public TOpt
{
public:
    TOptKeyGeneration(TMsg* parent);
    TOptKeyGeneration(char * &buf,  int &n,TMsg* parent);
    void setLifetime(uint32_t value);
    void setAlgorithmId(uint16_t value);
    uint16_t getAlgorithmId();
    int getSize();
    char *getDUID();
    char * storeSelf(char* buf);
    ~TOptKeyGeneration();

private:
    uint32_t Lifetime;
    uint16_t AlgorithmId;

    bool Valid;
};

#endif 
