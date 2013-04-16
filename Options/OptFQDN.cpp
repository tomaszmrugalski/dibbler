/*
* Dibbler - a portable DHCPv6
*
* authors: Adrien CLERC, Bahattin DEMIRPLAK, Gaëtant ELEOUET
*          Mickaël GUÉRIN, Lionel GUILMIN, Lauréline PROVOST
*          from ENSEEIHT, Toulouse, France
*
* released under GNU GPL v2 licence
*
*/

#include <sstream>
#include <string.h>
#include <string>
#include "Portable.h"
#include "OptFQDN.h"
#include "Logger.h"
#include "Portable.h"

using namespace std;

TOptFQDN::TOptFQDN(const std::string& domain, TMsg* parent)
    :TOpt(OPTION_FQDN, parent), fqdn_(domain), flag_N_(false), flag_O_(false),
     flag_S_(false) {
    // The O flag is always off in client messages.
    Valid = true;
}

TOptFQDN::TOptFQDN(const char * buf, int bufsize, TMsg* parent)
                :TOpt(OPTION_FQDN, parent) {
    Valid = false;

    // empty fqdn field is ok (section 4.2 of RFC4704), but it must have
    // bits field
    if (bufsize < 1) {
        Log(Warning) << "Truncated FQDN option received." << LogEnd;
        return;
    }

    // Extracting flags...
    unsigned char flags = *buf;
    flag_N_ = flags & FQDN_N;
    flag_S_ = flags & FQDN_S;
    flag_O_ = flags & FQDN_O;
    buf += 1;
    bufsize -= 1;

    //Extracting domain name
    fqdn_ = "";
    if (bufsize <= 255 ) {
        unsigned char tmplength;
        while (bufsize>0) {
            tmplength = *buf;
            buf++;
            bufsize--;
            if (tmplength>bufsize)
            {
                Log(Warning) << "Malformed FQDN option: domain name encoding is invalid." << LogEnd;
                return;
            }
            if (tmplength == 0) {
                buf += bufsize;
                bufsize = 0;
            } else {
                if (fqdn_.length())
                    fqdn_.append(".");
                fqdn_.append(buf, tmplength);
                buf += tmplength;
                bufsize -= tmplength;
            }
        }
        Valid = true;
    } else {
        Log(Warning) << "Too long FQDN option (len=" << bufsize << ") received." << LogEnd;
        return;
    }
    Log(Debug) << "FQDN: FQDN option received: fqdn name=" << (fqdn_.length() ? fqdn_ : "[empty]") << LogEnd;
}

TOptFQDN::~TOptFQDN() {
    return;
}

void TOptFQDN::setNFlag(bool flag) {
    flag_N_ = flag;
}

void TOptFQDN::setSFlag(bool flag) {
    flag_S_ = flag;
}

void TOptFQDN::setOFlag(bool flag) {
    flag_O_ = flag;
}

std::string TOptFQDN::getFQDN() const {
    return fqdn_;
}

/**
 * @brief returns option size
 *
 * Each dot will be removed from the string, and replaced with a length < 63
 * The first length and the final 0 will increased the fqdn string length by 2
 * We also have to add 4 for the header (option type and size) and 1 for the flags.
 * 2 + 1 + 4 = 7
 *
 * @return size of the option (without option header)
 */
size_t TOptFQDN::getSize() {
    if (fqdn_.length()) {
        //the final 0 should be present only for full fqdn, not for partial hostname
        //we distinguish between them by dot . presence
        if ((fqdn_.find('.',0) == string::npos) ||
            (fqdn_[fqdn_.length()-1] == '.') ) //ends with dot => do not add final 0, will be
                                                //made from existing dot final position
            return fqdn_.length() + 6;
        return fqdn_.length() + 7;  //contains '.' => full fqdn
        }
    else
        //the final 0 should be present only for non-empty filed
        //rfc4704 : A client MAY also leave the Domain Name field empty if it desires the
        //server to provide a name.
        return 5;
}

char * TOptFQDN::storeSelf(char *buffer) {
    // Type and size
    buffer = writeUint16(buffer, OptType);
    buffer = writeUint16(buffer, getSize()-4);
    //Flag Initialization
    *buffer = 0;
    if (flag_N_) {
        *buffer |= FQDN_N;
    }
    if (flag_S_) {
        *buffer |= FQDN_S;
    }
    if (flag_O_) {
        *buffer |= FQDN_O;
    }

    buffer++;

    //FQDN field
    if (fqdn_.empty()) {
        // no FQDN? Ok, we're done then
        return buffer;
    }

    string copy = fqdn_;
    bool endzero = false;
    std::string::size_type dotpos = copy.find('.', 0);
    while(dotpos != string::npos) {
        endzero = true; //data is full fqdn, append zero at the end

        // write length
        *buffer = dotpos;
        buffer++;

        // write label
        memcpy(buffer, copy.c_str(), dotpos);
        buffer += dotpos;
        copy = copy.substr(dotpos + 1, copy.length()); // substring
        dotpos = copy.find('.', 0);
    }

    *buffer = copy.length(); //copy.length()==0 => the data ends with dot. (full fqdn forced)
    buffer++;
    if (copy.empty()) {
        return buffer;
    }
    memcpy(buffer, copy.c_str(), copy.length());
    buffer += copy.length();

    if (endzero) {
        //add final 0 only if not assigned by converting the dot
        //i.e. the data is fqdn not already ended with dot.
        *buffer = 0;
        buffer++;
    }

    return buffer;
}

std::string TOptFQDN::getPlain() {
    stringstream tmp;

    tmp << fqdn_;
    if (flag_N_)
        tmp << " N";
    if (flag_O_)
        tmp << " O";
    if (flag_S_)
        tmp << " S";
    return tmp.str();
}

bool TOptFQDN::isValid() const {
    /// @todo Check the validity of this option
    return Valid;
}

bool TOptFQDN::getNFlag() const {
    return flag_N_;
}

bool TOptFQDN::getSFlag() const {
    return flag_S_;
}

bool TOptFQDN::getOFlag() const {
    return flag_O_;
}

bool TOptFQDN::doDuties() {
    return true;
}
