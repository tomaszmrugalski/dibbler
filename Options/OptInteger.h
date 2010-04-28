/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptInteger.h,v 1.1 2006-10-29 13:11:46 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:23:13  thomson
 * Initial revision
 *
 */

#ifndef OPTINTEGER4_H
#define OPTINTEGER4_H
#include <iostream>
#include <string>

#include "Opt.h"

using namespace std;

class TOptInteger : public TOpt
{
public:
    TOptInteger(int type, unsigned int len/* 1,2, or 4*/, unsigned int value, TMsg* parent);
    TOptInteger(int type, unsigned int len/* 1,2, or 4*/, char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    unsigned int getValue();
    bool isValid();
protected:
    unsigned int Value;
    bool Valid;
    int Len;    /* length in bytes of the integer field: 0-4 */
};

#endif
