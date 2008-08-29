/*
 * Dibbler - a portable DHCPv6
 *
 * author: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTAUTHENTICATION_H
#define OPTAUTHENTICATION_H

#include "DHCPConst.h"
#include "Opt.h"
#include "Portable.h"

class TOptAuthentication : public TOpt
{
public:
    TOptAuthentication(TMsg* parent);
    TOptAuthentication(char * &buf,  int &n,TMsg* parent);
    void setRDM(uint8_t value);
    uint32_t getSPI();
    void setAuthInfoLen(uint16_t len);
    void setDigestType(enum DigestTypes type);
    int getSize();
    char * storeSelf(char* buf);

private:
    uint8_t RDM;
    uint16_t AuthInfoLen;

    bool Valid;
};

#endif 
