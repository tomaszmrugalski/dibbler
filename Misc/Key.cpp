/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 licence
 */

#include <string.h>
#include "Key.h"
#include "base64.h"
#include "Logger.h"

TSIGKey::TSIGKey(const std::string& name)
    : Digest_(DIGEST_NONE), Name_(name), Fudge_(300) {

}
std::string TSIGKey::getAlgorithmText() {
    switch (Digest_) {
    case DIGEST_HMAC_MD5:
	return std::string("HMAC-MD5.SIG-ALG.REG.INT");
    case DIGEST_HMAC_SHA1:
	return std::string("HMAC-SHA1.SIG-ALG.REG.INT");
    case DIGEST_HMAC_SHA256:
	return std::string("HMAC-SHA256.SIG-ALG.REG.INT");
    default:
	return std::string("Unsupported algorithm");
    }
}

void TSIGKey::setData(const std::string& base64encoded) {
    Base64Data_ = base64encoded;

    char raw[100];
    unsigned raw_len = 100;
    memset(raw, 0, 100);

    base64_decode_context ctx = {0};
    base64_decode_ctx_init(&ctx);
    
    if (base64_decode(&ctx, Base64Data_.c_str(),
		      Base64Data_.length(),
		      raw, &raw_len)) {
    } else {
    }

    Data_.resize(raw_len);
    for (unsigned i=0; i<raw_len; i++) {
	Data_[i] = raw[i];
    }
}

std::string TSIGKey::getPackedData() {
    return Data_;
}

std::string TSIGKey::getBase64Data() {
    return Base64Data_;
}
