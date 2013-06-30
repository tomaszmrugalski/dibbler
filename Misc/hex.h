/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * Released under GNU GPL v2 licence
 *
 */

#ifndef HEX_H
#define HEX_H

#include <stdint.h>
#include <string>
#include <vector>

std::vector<uint8_t> textToHex(std::string buf);

std::string hexToText(const uint8_t* buf, size_t buf_len, bool add_colons = false,
                       bool add_0x = false);

std::string hexToText(const std::vector<uint8_t>& vector, bool add_colons = false,
                      bool add_0x = false);

#endif
