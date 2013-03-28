/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef OPTVENDORCLASS_H
#define OPTVENDORCLASS_H

#include "OptUserClass.h"
#include <stdint.h>

class TOptVendorClass : public TOptUserClass
{
 public:
    uint32_t Enterprise_id_;

    TOptVendorClass(uint16_t type, const char* buf, unsigned short buf_len, TMsg* parent);
    size_t getSize();
    char * storeSelf( char* buf);
};

#endif /* USERCLASS_H */
