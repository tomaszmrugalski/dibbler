/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#include <vector>
#include <sstream>
#include <iomanip>
#include "hex.h"
#include "Logger.h"

std::vector<uint8_t> textToHex(std::string buf) {
    // if it starts with 0x, just ignore that prefix
    if ( (buf.length() >= 2) && (buf[0] == '0') && (buf[1] == 'x')) {
        buf = buf.substr(2);
    }

    std::vector<uint8_t> tmp;

    int textLen = buf.length();
    unsigned char digit;
    int i=0;
    bool twonibbles = false;
    char value = 0;
    while (i < textLen)
    {
        if (buf[i]==':') {
            i++;
        }

        digit = buf[i];
        if (isalpha(digit))
            digit = toupper(digit)-'A'+10;
        else
            digit -= '0';
        value <<= 4;
        value |= digit;
        i++;
        if (twonibbles) {
            twonibbles = false;
            tmp.push_back(value);
            value = 0;
        } else
            twonibbles = true;
    }

    return tmp;
}



std::string hexToText(const uint8_t* buf, size_t buf_len, bool add_colons /*= false*/,
                      bool add_0x /* = false*/) {

    std::ostringstream tmp;
    if (add_0x)
        tmp << "0x";

    for(unsigned i = 0; i < buf_len; i++) {
        if (i)
            tmp << ":";
        tmp << std::setfill('0') << std::setw(2) << std::hex
            << (unsigned short)((unsigned char) buf[i]);
    }

    return tmp.str();
}

std::string hexToText(const std::vector<uint8_t>& vector, bool add_colons /*= false*/,
                      bool add_0x /*= false*/) {
    return hexToText(&vector[0], vector.size(), add_colons, add_0x);
}
