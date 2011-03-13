/*
* Dibbler - a portable DHCPv6
*
* authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
*          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
*          from ENSEEIHT, Toulouse, France
*
* released under GNU GPL v2 licence
*
* $Id: OptFQDN.cpp,v 1.8 2008-11-11 20:55:46 thomson Exp $
*
*/

#include <string.h>
#include "Portable.h"
#include "OptFQDN.h"
#include "Logger.h"

TOptFQDN::TOptFQDN(string domain, TMsg* parent)
		:TOpt(OPTION_FQDN, parent) {
    fqdn = domain;
    flag_N = false;
    flag_S = false;
    flag_O = false; // This flag is always off in client messages.

    Valid = true;
}

TOptFQDN::TOptFQDN(char * &buf, int &bufsize, TMsg* parent)
		:TOpt(OPTION_FQDN, parent) {
    this->Valid = false;
    // Extracting flags...
    unsigned char flags = *buf;
    this->flag_N = flags & FQDN_N;
    this->flag_S = flags & FQDN_S;
    this->flag_O = flags & FQDN_O;
    buf += 1;
    bufsize -= 1;

    //Extracting domain name
    fqdn = "";
    if ( bufsize <= 255 ) {
	unsigned char tmplength = *buf;
	if (tmplength>bufsize) 
	{
	    Log(Warning) << "Malformed FQDN option: domain name encoding is invalid. "
			 << "(Is this message sent by Microsoft? Tell them to fix the FQDN option.)" << LogEnd;
	    Valid = false;
	    return;
	}

	buf++;
	while (tmplength != 0) {
	    fqdn.append(buf, tmplength);
	    buf += tmplength;
	    bufsize -= tmplength;
	    tmplength = *buf;
	    if (tmplength>bufsize) 
	    {
		Log(Warning) << "Malformed FQDN option: domain name encoding is invalid."
			     << "(Is this message sent by Microsoft? Tell them to fix the FQDN option.)" << LogEnd;
		Valid = false;
		return;
	    }
	    buf++;
	    if ( tmplength != 0 ) {
		fqdn.append(".");
	    }
	}
	buf++;
	bufsize--;
	Valid = true;
    }
    Log(Debug) << "FQDN: FQDN option received: fqdn name=" << fqdn << LogEnd;
}

TOptFQDN::~TOptFQDN() {
	return;
}

void TOptFQDN::setNFlag(bool flag) {
	flag_N = flag;
}

void TOptFQDN::setSFlag(bool flag) {
	flag_S = flag;
}

void TOptFQDN::setOFlag(bool flag) {
	flag_O = flag;
}

string TOptFQDN::getFQDN() {
	return fqdn;
}

/** 
 * @brief returns option size
 *
 * Each dot will be removed from the string, and replaced with a length < 63
 * The first length and the final 0 will increased the fqdn string length by 2
 * We also have to add 4 for the header (option type and size) and 1 for the flags.
 * 2 + 1 + 4 = 7
 * 
 * 
 * 
 * @return size of the option (without option header)
 */
int TOptFQDN::getSize() {
        if (fqdn.length())
	    return fqdn.length() + 7;
	else
	    return 6;
}

char * TOptFQDN::storeSelf(char *buffer) {
	// Type and size
	*(short*)buffer = htons(OptType);
	buffer+=2;
	*(short*)buffer = htons(getSize()-4);
	buffer+=2;
//Flag Initialization
	*buffer = 0;
	if (flag_N) {
		*buffer += FQDN_N;
	}
	if (flag_S) {
		*buffer += FQDN_S;
	}
	if (flag_O) {
		*buffer += FQDN_O;
	}
	buffer++;

	//FQDN data :)
	if ( fqdn.length() != 0 ) {
		string copy = "";
		copy += fqdn;
	        std::string::size_type dotpos = copy.find('.', 0);
		while(dotpos != string::npos) {
			*buffer = dotpos;
			buffer++;
			memcpy(buffer, copy.c_str(), dotpos);
			buffer += dotpos;
			copy.assign(copy, dotpos + 1, copy.length());
			dotpos = copy.find('.', 0);
		}

		*buffer = copy.length();
		buffer++;
		memcpy(buffer, copy.c_str(), copy.length());
		buffer += copy.length();
	}
	*buffer = 0;
	//buffer++;

	return buffer;
}

bool TOptFQDN::isValid() {
    /// @todo Check the validity of this option
    return Valid;
}

bool TOptFQDN::getNFlag( ) {
    return flag_N;
}

bool TOptFQDN::getSFlag( ) {
    return flag_S;
}

bool TOptFQDN::getOFlag( ) {
    return flag_O;
}
