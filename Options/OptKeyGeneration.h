/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: OptKeyGeneration.h,v 1.2 2008-08-29 00:07:30 thomson Exp $
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
