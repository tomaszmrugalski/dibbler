/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptInteger4.h,v 1.1 2004-11-02 01:23:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef OPTINTEGER4_H
#define OPTINTEGER4_H
#include <iostream>
#include <string>

#include "Opt.h"

using namespace std;

class TOptInteger4 : public TOpt
{
public:
    TOptInteger4(int type, unsigned int value, TMsg* parent);
    TOptInteger4(int type, char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    unsigned int getValue();
    bool isValid();
protected:
    unsigned int Value;
    bool Valid;
};

#endif
