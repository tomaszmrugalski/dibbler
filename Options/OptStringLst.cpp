/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptStringLst.cpp,v 1.3 2005-03-07 23:36:14 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/03/07 22:44:22  thomson
 * DomainList is now stored and parsed approprietly (bug #104)
 *
 * Revision 1.1  2004/11/02 01:23:13  thomson
 * Initial revision
 *
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
    string domain = "";
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
        if (len==0) {
            // end of domain
            if (domain.length()) {
        	    SmartPtr<string> x = new string(domain);
                this->StringLst.append(x);
            }
            bufsize -= len+1;
            buf     += len+1;
            domain   = "";
            continue;
        }
	    memcpy(str,buf+1,len);
	    str[len]=0;

        Log(Debug) << "BEFORE: " << domain << LogEnd;
        if (domain.length())
            domain += string(".");
        domain = domain + string(str);
        Log(Debug) << "AFTER: " << domain << LogEnd;

	    bufsize -= len+1;
	    buf     += len+1;
    }
    delete [] str;
}

char * TOptStringLst::storeSelf(char* buf)
{
    SmartPtr<string> x;
    *(short*)buf = htons(OptType);
    buf+=2;
    *(short*)buf = htons(getSize()-4);
    buf+=2;
    char * bufStart = buf;
    int len = 0;
    int dotpos;
   
    StringLst.first();
    while (x = StringLst.get() ) {
        string cp(*x);
        Log(Debug) << "LEN: " << buf-bufStart << LogEnd;
        len = x->length();

        dotpos = -1;
        while (cp.find(".")!=-1) {
	    if (*cp.c_str()=='.')
		break;
            Log(Debug) << "TMP: " << cp << LogEnd;
            dotpos = cp.find(".");
            if (dotpos!=-1) {
                *buf = dotpos;
                buf++;
                memcpy(buf, cp.c_str(), dotpos);
                buf+=dotpos;
                cp = cp.substr(dotpos+1);
            }
            Log(Debug) << "dotpos=" << dotpos << LogEnd;   

        }

        *buf = cp.length(); // length of the string
        buf++;
        memcpy(buf, cp.c_str(), x->length());
	buf+= cp.length();
	*buf=0;
	buf++;
    }
    return buf;
}

int TOptStringLst::getSize() {
    int len = 0;
    int tmplen = 0;
    SmartPtr<string> x;
    StringLst.first();
    while ( x = StringLst.get() ) {
	Log(Debug) << "### GetSize()" << x->length() << LogEnd;
	const char * c = x->c_str();
	tmplen = x->length();
	if (c[tmplen]=='.')
	    len++;
	len += x->length()+2;
    }
    return len+4; // 5=4(std.option header) + 1 (final 0)
}
string TOptStringLst::getString() {
    return *(this->StringLst.get());
}
