/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptString.h,v 1.1 2004-11-02 01:23:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
 protected:
    string Str;
};

#endif
