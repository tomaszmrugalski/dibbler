/*
 * Dibbler - a portable DHCPv6
 *
 * author : Michal Kowalczuk <michal@kowalczuk.eu>
 * changes: Tomasz Mrugalski <thomson(at)klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: OptAAAAuthentication.h,v 1.2 2008-08-29 00:07:30 thomson Exp $
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
    TOptAAAAuthentication(TMsg* parent);
    TOptAAAAuthentication(char * &buf,  int &n,TMsg* parent);
    void setAAASPI(uint32_t value);
    uint32_t getAAASPI();
    int getSize();
    char * storeSelf(char* buf);

private:
    uint32_t AAASPI;

    bool Valid;
};

#endif 
