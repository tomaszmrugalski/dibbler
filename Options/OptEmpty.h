/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTRAPIDCOMMIT_H
#define OPTRAPIDCOMMIT_H

#include "DHCPConst.h"
#include "Opt.h"

class TOptEmpty : public TOpt
{
public:
    TOptEmpty(int code, TMsg* parent);
    TOptEmpty(int code, const char * buf,  int n, TMsg* parent);
    int getSize();
    char * storeSelf(char* buf);
    bool doDuties() { return false; }
};

#endif 
