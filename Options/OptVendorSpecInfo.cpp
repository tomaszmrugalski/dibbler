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
#include <iostream>
#include <sstream>
#include "Portable.h"
#include "OptVendorSpecInfo.h"
#include "OptGeneric.h"
#include "DHCPConst.h"
#include "Logger.h"

#if defined(LINUX) || defined(BSD)
#include <arpa/inet.h>
#endif

TOptVendorSpecInfo::TOptVendorSpecInfo(uint16_t type, char * buf,  int n, TMsg* parent)
    :TOpt(type, parent), Vendor_(0)
{
    int optionCode = 0, optionLen = 0;
    if (n<4) {
	Log(Error) << "Unable to parse truncated vendor-spec info option." << LogEnd;
        Valid = false;
	return;
    }

    Vendor_ = readUint32(buf); // enterprise number
    buf += sizeof(uint32_t);
    n   -= sizeof(uint32_t);

    while (n>=4) {
        optionCode = readUint16(buf);
        buf += sizeof(uint16_t); n -= sizeof(uint16_t);
        optionLen  =  readUint16(buf);
        buf += sizeof(uint16_t); n -= sizeof(uint16_t);
        if (optionLen>n) {
            Log(Warning) << "Malformed vendor-spec info option. Suboption " << optionCode
                         << " truncated." << LogEnd;
            Valid = false;
            return;
        }

        SPtr<TOpt> opt = new TOptGeneric(optionCode, buf, optionLen, parent);
        addOption(opt);
        buf += optionLen;
        n   -= optionLen;
    }
    if (n) {
        Log(Warning) << "Extra " << n << " bytes, after parsing suboption " << optionCode
                     << " in vendor-spec info option." << LogEnd;
        Valid = false;
        return;
    }
    Valid = true;
}

TOptVendorSpecInfo::TOptVendorSpecInfo(uint16_t code, uint32_t enterprise, uint16_t sub_option_code,
                                       const char *data, int dataLen, TMsg* parent)
    :TOpt(code, parent), Vendor_(enterprise)
{
    createSuboption(sub_option_code, data, dataLen);
}

void TOptVendorSpecInfo::createSuboption(uint16_t sub_option_code, const char* data,
                                    size_t data_len) {
    if (sub_option_code) {
        SPtr<TOptGeneric> opt = new TOptGeneric(sub_option_code, data, data_len, Parent);
        addOption( (Ptr*) opt);
    }
}

TOptVendorSpecInfo::TOptVendorSpecInfo(uint16_t code, uint32_t enterprise, uint16_t sub_option_code,
                                       SPtr<TIPv6Addr> addr, TMsg* parent)
    :TOpt(code, parent), Vendor_(enterprise)
{
    createSuboption(sub_option_code, addr->getAddr(), 16);
}

TOptVendorSpecInfo::TOptVendorSpecInfo(uint16_t code, uint32_t enterprise, uint16_t sub_option_code,
                                       const std::string& str, TMsg* parent)
    :TOpt(code, parent), Vendor_(enterprise)
{
    createSuboption(sub_option_code, str.c_str(), str.length());
}

TOptVendorSpecInfo::~TOptVendorSpecInfo() 
{
}

size_t TOptVendorSpecInfo::getSize() {
    SPtr<TOpt> opt;
    unsigned int len = 8; // normal header(4) + enterprise(4)
    firstOption();
    while (opt = getOption()) {
        len += opt->getSize();
    }
    return len;
}

char * TOptVendorSpecInfo::storeSelf( char* buf)
{
    // option-code OPTION_VENDOR_OPTS (2 bytes long)
    buf = writeUint16(buf, OptType);

    // option-len size of total option-data
    buf = writeUint16(buf, getSize()-4);

    // enterprise-number (4 bytes long)
    buf = writeUint32(buf, Vendor_);

    SPtr<TOpt> opt;
    firstOption();
    while (opt = getOption())
    {
        buf = opt->storeSelf(buf);
    }
    
    return buf;
}

std::string TOptVendorSpecInfo::getPlain() {
    std::stringstream tmp;
    tmp << "vendor=" << Vendor_ << " ";

    SPtr<TOpt> opt;
    firstOption();
    while (opt = getOption())
    {
	tmp << opt->getOptType() << "=";
	tmp << opt->getPlain() << " ";
    }
    return tmp.str();
}

bool TOptVendorSpecInfo::isValid() const
{
    return true;
}

uint32_t TOptVendorSpecInfo::getVendor()
{
    return Vendor_;
}
