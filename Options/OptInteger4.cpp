/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptInteger4.cpp,v 1.1 2004-11-02 01:23:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/04/11 18:10:56  thomson
 * CRLF fixed.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 */

#include <stdlib.h>
#include <iostream>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif
#include "OptInteger4.h"
#include "DHCPConst.h"

using namespace std;

TOptInteger4::TOptInteger4(int type, unsigned int value, TMsg* parent)
    :TOpt(type, parent) {
    this->Value = value;
    this->Valid = true;
}

TOptInteger4::TOptInteger4(int type, char *&buf, int &bufsize, TMsg* parent)
    :TOpt(type, parent)
{
    if (bufsize<4) {
	this->Valid = false;
	return;
    }
    this->Value = ntohl(*(int*)buf);
    buf += 4;
    this->Valid = true;
}

char * TOptInteger4::storeSelf(char* buf)
{
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(4);
    buf+=2;    
    *(int*)buf = htonl(this->Value);
    return buf+4;    

}
int TOptInteger4::getSize() {
    return 8; 
}

unsigned int TOptInteger4::getValue() {
    return this->Value;
}

bool TOptInteger4::isValid() {
    return this->Valid;
}
