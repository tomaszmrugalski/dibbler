/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 */

#include "Key.h"

TSIGKey::TSIGKey(const std::string& name)
    : Digest_(DIGEST_NONE), Name_(name), Fudge_(300) {

}
std::string TSIGKey::getAlgorithmText() {
    switch (Digest_) {
    case DIGEST_HMAC_MD5:
	return std::string("HMAC-MD5.SIG-ALG.REG.INT");
    case DIGEST_HMAC_SHA256:
	return std::string("HMAC-SHA256.SIG-ALG.REG.INT");
    default:
	return std::string("Unsupported algorithm");
    }
}

