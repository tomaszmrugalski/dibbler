/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#ifndef OPTINTEGER4_H
#define OPTINTEGER4_H
#include <iostream>
#include <string>

#include "Opt.h"

class TOptInteger : public TOpt
{
public:
    TOptInteger(uint16_t type, unsigned int len/* 1,2, or 4*/, unsigned int value, TMsg* parent);
    TOptInteger(uint16_t type, unsigned int len/* 1,2, or 4*/, const char *buf, size_t size, TMsg* parent);
    char * storeSelf( char* buf);
    size_t getSize();
    unsigned int getValue();
    bool isValid() const;
    std::string getPlain();
    bool doDuties() { return true; }
protected:
    unsigned int Value;
    bool Valid;
    int Len;    /* length in bytes of the integer field: 0-4 */
};

#endif
