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

class TSIGKey {
public:
TSIGKey(const std::string& name)
    : Digest_(DIGEST_NONE), Name_(name), Fudge_(300) {}
    std::string getAlgorithmText() {
	switch (Digest_) {
	case DIGEST_HMAC_MD5:
	    return std::string("HMAC-MD5.SIG-ALG.REG.INT");
	case DIGEST_HMAC_SHA256:
	    return std::string("HMAC-SHA256.SIG-ALG.REG.INT");
	default:
	    return std::string("Unsupported algorithm");
	}
    }

    DigestTypes Digest_;
    std::string Name_;
    std::string Data_;
    uint16_t Fudge_;
};

typedef std::vector< SPtr<TSIGKey> > TSIGKeyList;

#endif /* KEY_H */
