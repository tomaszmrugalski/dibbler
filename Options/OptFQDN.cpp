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

#include <string.h>
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

TOptFQDN::TOptFQDN(char * &buf, int &bufsize, TMsg* parent)
                :TOpt(OPTION_FQDN, parent) {
    Valid = false;

    if (bufsize < 2) {
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
    if ( bufsize <= 255 ) {
        unsigned char tmplength = *buf;
        if (tmplength>bufsize)
        {
            Log(Warning) << "Malformed FQDN option: domain name encoding is invalid. "
                         << "(Is this message sent by Microsoft? Tell them to fix the FQDN option.)" << LogEnd;
            return;
        }

        buf++;
        while (tmplength != 0) {
            fqdn_.append(buf, tmplength);
            buf += tmplength;
            bufsize -= tmplength;
            tmplength = *buf;
            if (tmplength>bufsize)
            {
                Log(Warning) << "Malformed FQDN option: domain name encoding is invalid."
                             << "(Is this message sent by Microsoft Vista client? Tell them to fix the FQDN option.)" << LogEnd;
                return;
            }
            buf++;
            if ( tmplength != 0 ) {
                fqdn_.append(".");
            }
        }
        buf++;
        bufsize--;
        Valid = true;
    } else {
        Log(Warning) << "Too long FQDN option (len=" << bufsize << ") received." << LogEnd;
        return;
    }
    Log(Debug) << "FQDN: FQDN option received: fqdn name=" << fqdn_ << LogEnd;
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
    if (fqdn_.length())
        return fqdn_.length() + 7;
    else
        return 6;
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

    //FQDN data :)
    if ( fqdn_.length() != 0 ) {
        string copy = "";
        copy += fqdn_;
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

    return buffer;
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
