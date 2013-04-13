/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 */

#ifndef KEY_H
#define KEY_H

#include <vector>
#include "SmartPtr.h"
#include "DHCPConst.h"
#include "Portable.h"

typedef std::vector<uint8_t> TKey;

class TSIGKey {
public:
    TSIGKey(const std::string& name);
    std::string getAlgorithmText();

    bool setData(const std::string& base64encoded);
    std::string getPackedData();
    std::string getBase64Data();

    DigestTypes Digest_;
    std::string Name_;
    uint16_t Fudge_;
protected:
    std::string Data_;
    std::string Base64Data_;
};

typedef std::vector< SPtr<TSIGKey> > TSIGKeyList;

#endif /* KEY_H */
