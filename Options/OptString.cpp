/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptString.cpp,v 1.3 2007-08-26 10:26:19 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005-07-17 21:09:52  thomson
 * Minor improvements for 0.4.1 release.
 *
 * Revision 1.1  2004/11/02 01:23:13  thomson
 * Initial revision
 *
 * Revision 1.3  2004/04/11 18:10:56  thomson
 * CRLF fixed.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include <stdlib.h>
#include <iostream>
#include "Portable.h"
#include "OptString.h"
#include "DHCPConst.h"

using namespace std;

TOptString::TOptString(int type, string str, TMsg* parent)
    :TOpt(type, parent) {
    Str = str;
}

TOptString::TOptString(int type, char *&buf, int &bufsize, TMsg* parent)
    :TOpt(type, parent)
{
    char* str=new char[bufsize+1];
    memcpy(str,buf,bufsize);
    str[bufsize]=0;
    Str = string(str);
    delete [] str;
    bufsize -= this->Str.length()+1;
    buf += this->Str.length()+1;
}

char * TOptString::storeSelf(char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;    
    memcpy(buf,Str.c_str(),Str.length());
    buf[Str.length()]=0;  // null-terminated
    return buf+this->Str.length()+1;

    *buf = (char)Str.length(); // length of a string (with first byte)
    buf++;
    memcpy(buf,this->Str.c_str(),this->Str.length());
    buf += this->Str.length();
    *buf=0; // add final 0.
    return buf+1;
}

int TOptString::getSize() {
    return (int)(Str.length()+6); // 4-normal header + 1 (strlen) + 1 (final 0)
}

string TOptString::getString() {
    return Str;
}
