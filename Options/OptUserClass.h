/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef USERCLASS_H
#define USERCLASS_H

#include "Opt.h"
#include "DHCPConst.h"


class TOptUserClass : public TOpt
{
 public:
    TOptUserClass( char * &buf,  int &n, TMsg* parent);
    
    int getSize();
    bool isValid();
    char * storeSelf( char* buf);
};

#endif /* USERCLASS_H */
