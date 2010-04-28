/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#ifndef OPTSTRING_H
#define OPTSTRING_H
#include <iostream>
#include <string>

#include "Opt.h"

using namespace std;

class TOptString : public TOpt
{
 public:
    TOptString(int type, string str, TMsg* parent);
    TOptString(int type, char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    string getString();
    virtual bool doDuties() { return true; } // do nothing, actual code in ClntOpt* classes
 protected:
    string Str;
};

#endif
