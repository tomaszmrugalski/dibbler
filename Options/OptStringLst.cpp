/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptStringLst.cpp,v 1.1 2004-11-02 01:23:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004/04/11 18:10:56  thomson
 * CRLF fixed.
 *
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
#include "OptStringLst.h"
#include "DHCPConst.h"
#include "Logger.h"

using namespace std;

TOptStringLst::TOptStringLst(int type, List(string) strLst, TMsg* parent)
    :TOpt(type, parent)
{
    this->StringLst = strLst;
}

TOptStringLst::TOptStringLst(int type, char *&buf, int &bufsize, TMsg* parent)
    :TOpt(type, parent) {
    int len;
    char* str=new char[bufsize+1];
    str[bufsize]=0;
    while (bufsize) {
	len = (char)*buf;
	if (len>bufsize) {
	    Log(Debug) << "Option parsing failed. String length is specified as " << len
		       << ", but remaining buffer is only " << bufsize << " bytes long." << LogEnd;
	    StringLst.clear();
	    return;
	}
	if (len==0) 
	    break;
	memcpy(str,buf+1,len);
	str[len]=0;
	SmartPtr<string> x = new string(str);
	this->StringLst.append(x);
	bufsize -= len+1;
	buf     += len+1;
    }
    delete [] str;
    if (bufsize<1 || *buf!=0 ) {
	Log(Debug) << "Option parsing failed. String parsing failed without final 0." << LogEnd;
	StringLst.clear();
	return;
    }
    if (bufsize>0) {
	bufsize-= 1;
	buf    += 1;
    }
}

char * TOptStringLst::storeSelf(char* buf)
{
    SmartPtr<string> x;
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;

    StringLst.first();
    while (x = StringLst.get() ) {
	*buf = x->length(); // length of the string
	buf++;
	memcpy(buf, x->c_str(), x->length());
	buf+= x->length();
    }
    *buf = 0; // add final 0
    return buf+1;
}

int TOptStringLst::getSize() {
    int len = 0;
    SmartPtr<string> x;
    StringLst.first();
    while ( x = StringLst.get() ) {
	len += x->length()+1;
    }
    return len+5; // 5=4(std.option header) + 1 (final 0)
}
string TOptStringLst::getString() {
    return *(this->StringLst.get());
}
