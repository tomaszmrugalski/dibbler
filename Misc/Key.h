/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 */

#include "DHCPConst.h"

class TKey {
 public:
 TKey(std::string name_): digest(DIGEST_NONE), name(name_), data(0), len(0) {}
    DigestTypes digest;
    std::string name;
    unsigned char * data;
    unsigned int len;
};
