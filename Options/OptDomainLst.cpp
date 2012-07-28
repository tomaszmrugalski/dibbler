/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 */

#include <string.h>
#include "Portable.h"
#include "OptDomainLst.h"
#include "DHCPConst.h"
#include "Logger.h"

using namespace std;

TOptDomainLst::TOptDomainLst(int type, List(std::string) strLst, TMsg* parent)
    :TOpt(type, parent) {
    StringLst = strLst;
}

TOptDomainLst::TOptDomainLst(int type, const std::string& domain, TMsg* parent)
    :TOpt(type, parent) {
    StringLst.append(new string(domain) );
}

const std::string& TOptDomainLst::getDomain() {
    StringLst.first();
    return *StringLst.get();
}


TOptDomainLst::TOptDomainLst(int type, const char *buf, unsigned short bufsize, TMsg* parent)
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
        	    SPtr<string> x = new string(domain);
                this->StringLst.append(x);
            }
            bufsize -= len+1;
            buf     += len+1;
            domain   = "";
            continue;
        }
	    memcpy(str,buf+1,len);
	    str[len]=0;

        if (domain.length())
            domain += string(".");
        domain = domain + string(str);

	    bufsize -= len+1;
	    buf     += len+1;
    }
    delete [] str;
}

char * TOptDomainLst::storeSelf(char* buf)
{
    SPtr<string> x;
    buf = writeUint16(buf, OptType);
    buf = writeUint16(buf, getSize()-4);
    std::string::size_type dotpos;
   
    StringLst.first();
    while (x = StringLst.get() ) {
        string cp(*x);

        dotpos = string::npos;
        while (cp.find(".")!=string::npos) {
	    if (*cp.c_str()=='.')
		break;
            dotpos = cp.find(".");
            if (dotpos!=string::npos) {
                *buf = dotpos;
                buf++;
                memcpy(buf, cp.c_str(), dotpos);
                buf+=dotpos;
                cp = cp.substr(dotpos+1);
            }
        }

        *buf = cp.length(); // length of the string
        buf++;
        memcpy(buf, cp.c_str(), cp.length());
	buf+= cp.length();
	*buf=0;
	buf++;
    }
    return buf;
}

size_t TOptDomainLst::getSize() {
    int len = 0;
    int tmplen = 0;
    SPtr<string> x;
    StringLst.first();
    while ( x = StringLst.get() ) {
	const char * c = x->c_str();
	tmplen = x->length();
	if (c[tmplen]=='.')
	    len++;
	len += x->length()+2;
    }
    return len+4; // 5=4(std.option header) + 1 (final 0)
}

#if 0
string TOptDomainLst::getString() {
    return *(this->StringLst.get());
}
#endif

std::string TOptDomainLst::getPlain() {
    string concat;

    StringLst.first();
    SPtr<string> s;
    while (s=StringLst.get()) {
        concat.append(*s);
        concat.append(" ");
    }
    return concat;
}
