/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptRapidCommit.cpp,v 1.3 2005-07-17 19:56:55 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#ifdef WIN32
#include <winsock2.h>
#endif

#include "OptEmpty.h"

TOptEmpty::TOptEmpty(int code, const char * buf,  int n, TMsg* parent)
    :TOpt(code, parent) {
}

TOptEmpty::TOptEmpty(int code, TMsg* parent)
    :TOpt(code, parent) {
}

 int TOptEmpty::getSize() {
     return 4;
}

 char * TOptEmpty::storeSelf( char* buf) {
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    return buf+2;
}
