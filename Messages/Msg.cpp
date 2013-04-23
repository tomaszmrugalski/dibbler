/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 */

#include <string.h>
#include <stdlib.h>
#include <cmath>
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"
#include "Msg.h"
#include "Opt.h"
#include "OptAuthentication.h"
#include "Logger.h"
#include "hmac-sha-md5.h"

class TNotifyScriptParams;

TMsg::TMsg(int iface, SPtr<TIPv6Addr> addr, char* &buf, int &bufSize)
    :NotifyScripts(NULL)
{
    setAttribs(iface, addr, 0, 0);
    if (bufSize<4)
	return;
    this->MsgType=buf[0];
    unsigned char * buf2 = (unsigned char *)(buf+1);
    this->TransID= ((long)buf2[0])<<16 | ((long)buf2[1])<<8 | (long)buf2[2];
    buf+=4; bufSize-=4;
}

TMsg::TMsg(int iface, SPtr<TIPv6Addr> addr, int msgType)
    :NotifyScripts(NULL)
{
    long tmp = rand() % (255*255*255);
    setAttribs(iface,addr,msgType,tmp);
}

TMsg::TMsg(int iface, SPtr<TIPv6Addr> addr, int msgType,  long transID)
    :NotifyScripts(NULL)
{
    setAttribs(iface,addr,msgType,transID);
}

void TMsg::setAttribs(int iface, SPtr<TIPv6Addr> addr, int msgType, long transID)
{
    PeerAddr = addr;

    Iface = iface;
    TransID = transID;
    IsDone = false;
    MsgType = msgType;
    DigestType_ = DIGEST_NONE; /* by default digest is none */
    AuthDigestPtr_ = NULL;
    AuthDigestLen_ = 0;
    SPI_ = 0;

#if AUTH_CRAP
    KeyGenNonce = NULL;
    KeyGenNonceLen = 0;
    AAASPI = 0;
#endif
}

int TMsg::getSize()
{
    int pktsize=0;
    TOptList::iterator opt;
    for (opt = Options.begin(); opt!=Options.end(); ++opt)
    {
	pktsize += (*opt)->getSize();
    }
    return pktsize + 4;
}

unsigned long TMsg::getTimeout()
{
	return 0;
}

long TMsg::getType()
{
    return MsgType;
}

long TMsg::getTransID()
{
    return TransID;
}

TOptList & TMsg::getOptLst()
{
    return Options;
}

/* prepares binary version of this message, returns number of bytes used
**/
int TMsg::storeSelf(char * buffer)
{
    char *start = buffer;
    int tmp = this->TransID;
    
    *(buffer++) = (char)MsgType;
    
    /* ugly 3-byte version of htons/htonl */
    buffer[2] = tmp%256;  tmp = tmp/256;
    buffer[1] = tmp%256;  tmp = tmp/256;
    buffer[0] = tmp%256;  tmp = tmp/256;
    buffer+=3;

    TOptList::iterator option;
    for (option=Options.begin(); option!=Options.end(); ++option) {
        (*option)->storeSelf(buffer);
	buffer += (*option)->getSize();
    }

#ifndef MOD_DISABLE_AUTH
    calculateDigests(start, buffer - start);
#endif

    return buffer-start;
}

void TMsg::calculateDigests(char* buffer, size_t len) {

    SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
    if (!auth)
        return;

    switch (auth->getProto()) {
    default: {
        Log(Error) << "Auth: protocol " << auth->getProto() << " not supported yet." << LogEnd;
        return;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: {
        if (AuthKey_.size() != RECONFIGURE_KEY_SIZE) {
            Log(Error) << "Auth: Invalid size of reconfigure-key: expected " <<
                RECONFIGURE_KEY_SIZE << ", actual size " << AuthKey_.size() << LogEnd;
            return;
        }
        if (auth->getAuthDataPtr()) {
            hmac_md5(buffer, len, (char*) &AuthKey_[0], AuthKey_.size() ,auth->getAuthDataPtr());
        }
        return;
    }
    case AUTH_PROTO_NONE: {
        // don't calculate anything
        return;
    }
    case AUTH_PROTO_DIBBLER: {
        DigestTypes UsedDigestType;

	// for AAAAUTH use only HMAC-SHA1
        UsedDigestType = DigestType_;

        if (getSPI() && !loadAuthKey()) {
            return;
        }
        
        if (getSPI() && !AuthKey_.empty() && UsedDigestType != DIGEST_NONE) {
            Log(Debug) << "Auth: Used digest type is " << getDigestName(UsedDigestType) << LogEnd;
            switch (UsedDigestType) {
            case DIGEST_PLAIN:
                memcpy(AuthDigestPtr_, "This is 32-byte plain testkey...", getDigestSize(UsedDigestType));
                break;
            case DIGEST_HMAC_MD5:
                hmac_md5(buffer, len, (char*)&AuthKey_[0], AuthKey_.size(), (char *)AuthDigestPtr_);
                break;
            case DIGEST_HMAC_SHA1:
                hmac_sha(buffer, len, (char*)&AuthKey_[0], AuthKey_.size(), (char *)AuthDigestPtr_, 1);
                break;
            case DIGEST_HMAC_SHA224:
                hmac_sha(buffer, len, (char*)&AuthKey_[0], AuthKey_.size(), (char *)AuthDigestPtr_, 224);
                break;
            case DIGEST_HMAC_SHA256:
                hmac_sha(buffer, len, (char*)&AuthKey_[0], AuthKey_.size(), (char *)AuthDigestPtr_, 256);
                break;
            case DIGEST_HMAC_SHA384:
                hmac_sha(buffer, len, (char*)&AuthKey_[0], AuthKey_.size(), (char *)AuthDigestPtr_, 384);
                break;
            case DIGEST_HMAC_SHA512:
                hmac_sha(buffer, len, (char*)&AuthKey_[0], AuthKey_.size(), (char *)AuthDigestPtr_, 512);
                break;
            default:
                break;
            }
            PrintHex(std::string("Auth: Sending digest ") + getDigestName(UsedDigestType) +" : ",
				 (uint8_t*)AuthDigestPtr_, getDigestSize(UsedDigestType));
        }
    }
    }

}

SPtr<TOpt> TMsg::getOption(int type) {
    TOptList::iterator opt;
    for (opt = Options.begin(); opt!=Options.end(); ++opt)
        if ( (*opt)->getOptType()==type) 
	    return *opt;
    return 0;
}

void TMsg::firstOption() {
    NextOpt = Options.begin();
}

int TMsg::countOption() {
    return Options.size();
}

SPtr<TOpt> TMsg::getOption() {
    if (NextOpt != Options.end()) {
	TOptList::iterator it = NextOpt;
	++NextOpt;
	return (*it);
    }
    return 0;
}

TMsg::~TMsg() {
    if (NotifyScripts) {
        delete NotifyScripts;
    }
}

SPtr<TIPv6Addr> TMsg::getAddr() {
    return PeerAddr;
}

int TMsg::getIface() {
    return Iface;
}

bool TMsg::isDone() {
    return IsDone;
}

bool TMsg::isDone(bool done) {
    IsDone = done;
    return IsDone;
}

void TMsg::setAuthDigestPtr(char* ptr, unsigned len) {
    AuthDigestPtr_ = ptr;
    AuthDigestLen_ = len;
}

void TMsg::setAuthKey(const TKey& key) {
    AuthKey_ = key;
    PrintHex("Auth: setting key to: ", (uint8_t*)&AuthKey_[0], AuthKey_.size());
}

bool TMsg::loadAuthKey() {

    unsigned len = 0;
    char * ptr = getAAAKey(SPI_, &len);
    AuthKey_.resize(len);
    memcpy(&AuthKey_[0], ptr, len);
    free(ptr);

    return (len>0);
    
#ifdef AUTH_CRAP
    //  key = HMAC-SHA1 (AAA-key, {Key Generation Nonce || client identifier})

    char *KeyGenNonce_ClientID;
    char * AAAkey;
    uint32_t AAAkeyLen;

    if (!KeyGenNonce)
        KeyGenNonceLen = 16;

    /// @todo set proper size of Client ID (DUID?) (here and in hmac_sha())
    KeyGenNonce_ClientID = new char[KeyGenNonceLen+128];

    AAAkey = getAAAKey(AAASPI, &AAAkeyLen);
    std::string fname = getAAAKeyFilename(AAASPI);

    // error, no file?
    if (!AAAkey) {
        Log(Error) << "Auth: Unable to load key file for SPI " << std::hex << AAASPI <<": " << fname 
                   << " not found." << std::dec << LogEnd;
        AuthKeyPtr_ = NULL;
        delete [] KeyGenNonce_ClientID;
        return -1;
    }
    Log(Debug) << "Auth: AAA-key loaded from file " << fname << "." << LogEnd;

    PrintHex("Auth: AAA-key: ", (uint8_t*)AAAkey, AAAkeyLen);

    memset(KeyGenNonce_ClientID, 0, KeyGenNonceLen+128);
    if (KeyGenNonce)
        memcpy(KeyGenNonce_ClientID, KeyGenNonce, KeyGenNonceLen);

    /// @todo fill also with ClientID (DUID?)

    PrintHex("Auth: Infokey: using KeyGenNonce+CliendID: ", (uint8_t*)KeyGenNonce_ClientID, KeyGenNonceLen+128);

    Log(Debug) << "Auth: AAAKeyLen: " << AAAkeyLen << ", KeyGenNonceLen: " << KeyGenNonceLen << LogEnd;
    AuthKeyPtr_ = new char[AUTHKEYLEN];
    hmac_sha(KeyGenNonce_ClientID, KeyGenNonceLen+128, AAAkey, AAAkeyLen, (char *)AuthKeyPtr_, 1);

    PrintHex("Auth: AuthKeyPtr_ (calculated): ", (uint8_t*)AuthKeyPtr_, AUTHKEYLEN);

    delete [] KeyGenNonce_ClientID;
#endif

    return 0;
}

TKey TMsg::getAuthKey() {
    return AuthKey_;
}

void TMsg::setSPI(uint32_t val) {
    SPI_ = val;
}

uint32_t TMsg::getSPI() {
    return SPI_;
}

#ifdef AUTH_CRAP
void TMsg::setAAASPI(uint32_t val) {
    AAASPI = val;
}

uint32_t TMsg::getAAASPI() {
    return AAASPI;
}

void TMsg::setKeyGenNonce(char *value, unsigned len)
{
    if (len) {
    	KeyGenNonce = new char[len];
        memcpy(KeyGenNonce, value, len);
        KeyGenNonceLen = len;
    }
}

char *TMsg::getKeyGenNonce() {
    return KeyGenNonce;
}

unsigned TMsg::getKeyGenNonceLen() {
    return KeyGenNonceLen;
}
#endif

bool TMsg::validateAuthInfo(char *buf, int bufSize, const DigestTypesLst& acceptedDigestTypes) {
    bool is_ok = false;
    bool dt_in_list = false;
    
    //empty list means that any digest type is accepted
    if (acceptedDigestTypes.empty()) {
        dt_in_list = true;
    } else {
        // check if the digest is allowed AUTH list
        for (unsigned i = 0; i < acceptedDigestTypes.size(); ++i) {
            if (acceptedDigestTypes[i] == DigestType_) {
                dt_in_list = true;
                break;
            }
        }
    }

    if (dt_in_list == false) {
        if (DigestType_ == DIGEST_NONE)
            Log(Error) << "Authentication option is required." << LogEnd;
        else
            Log(Error) << "Authentication method " << getDigestName(DigestType_) << " not accepted." << LogEnd;
        return false;
    }

    if (DigestType_ == DIGEST_NONE) {
            is_ok = true;
    } else if (AuthDigestPtr_) {
#ifndef MOD_DISABLE_AUTH

        if (AuthKey_.empty()) {
            Log(Debug) << "Auth: No AuthKey was set. This could mean bad SPI or no AAA-SPI file." << LogEnd;
            return false;
        }

        unsigned AuthInfoLen = getDigestSize(DigestType_);
        char *rcvdAuthInfo = new char[AuthInfoLen];
        char *goodAuthInfo = new char[AuthInfoLen];

        memmove(rcvdAuthInfo, AuthDigestPtr_, AuthInfoLen);
        memset(AuthDigestPtr_, 0, AuthInfoLen);

        switch (DigestType_) {
                // [s] change the key to something that make sense
                case DIGEST_PLAIN:
                    memcpy(goodAuthInfo, "This is 32-byte plain testkey...", 32);
                    break;
                case DIGEST_HMAC_MD5:
                    hmac_md5(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo);
                    break;
                case DIGEST_HMAC_SHA1:
                    hmac_sha(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo, 1);
                    break;
                case DIGEST_HMAC_SHA224:
                    hmac_sha(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo, 224);
                    break;
                case DIGEST_HMAC_SHA256:
                    hmac_sha(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo, 256);
                    break;
                case DIGEST_HMAC_SHA384:
                    hmac_sha(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo, 384);
                    break;
                case DIGEST_HMAC_SHA512:
                    hmac_sha(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo, 512);
                    break;
                default:
                    break;
        }
        if (0 == memcmp(goodAuthInfo, rcvdAuthInfo, AuthInfoLen))
            is_ok = true;

        Log(Debug) << "Auth:Checking using digest method: " 
                   << getDigestName(DigestType_) << LogEnd;
        PrintHex("Auth:received digest: ", (uint8_t*)rcvdAuthInfo, AuthInfoLen);
        PrintHex("Auth:  proper digest: ", (uint8_t*)goodAuthInfo, AuthInfoLen);

        delete [] rcvdAuthInfo;
        delete [] goodAuthInfo;

        if (is_ok)
            Log(Info) << "Auth: Digest correct." << LogEnd;
        else {
            Log(Warning) << "Auth: Digest incorrect." << LogEnd;
	}
#endif
    } else {
      Log(Error) << "Auth: Digest mode set to " << DigestType_
                 << ", but AUTH option not set." << LogEnd;
    }
    
    return is_ok;
}

/** 
 * checks if appropriate number of server/client IDs has been attached
 * 
 * @param srvIDmandatory - is ServerID option mandatory? (false==ServerID not allowed)
 * @param clntIDmandatory - is ClientID option mandatory?(false==ClientID is optional)
 * 
 * @return 
 */
bool TMsg::check(bool clntIDmandatory, bool srvIDmandatory)
{
    SPtr<TOpt> option;
    int clntCnt=0;
    int srvCnt =0;
    int authCnt = 0;
    bool status = true;

    for (TOptList::iterator opt=Options.begin(); opt!=Options.end(); ++opt)
    {
	switch ( (*opt)->getOptType() ) {
	case OPTION_CLIENTID:
	    clntCnt++;
	    break;
	case OPTION_SERVERID:
	    srvCnt++;
	    break;
	case OPTION_AUTH:
	    authCnt++;
	    break;
	default:
	    break;
	    /* ignore the rest */
	}
    }

    if (clntIDmandatory && (clntCnt!=1) ) {
	Log(Warning) << "Exactly 1 ClientID option required in the " << this->getName() 
		     << " message, but " << clntCnt << " received.";
	status = false;
    }

    if (srvIDmandatory && (srvCnt!=1) ) {
	Log(Warning) << "Exactly 1 ServerID option required in the " << this->getName() 
		     << " message, but " << srvCnt << " received.";
	status = false;
    }
    
    if (!srvIDmandatory && (srvCnt)) {
	Log(Warning) << "No ServerID option is allowed in the " << this->getName()
		     << " message, but " << srvCnt << " received.";
	status = false;
    }

    if (authCnt > 1) {
	Log(Warning) << "No more that one authentication option is allowed in the " << this->getName()
		     << " message, but " << srvCnt << " received.";
	status = false;
    }

    if (!status) {
	Log(Cont) << "Message dropped." << LogEnd;
	IsDone = true;
    }

    return status;
}

bool TMsg::delOption(int code)
{
    for (TOptList::iterator opt = Options.begin(); opt!=Options.end(); ++opt)
    {
	if ( (*opt)->getOptType() == code) {
	    Options.erase(opt);
	    firstOption();
	    return true;
	}
    }
    return false;
}

void* TMsg::getNotifyScriptParams() {
    if (!NotifyScripts) {
	NotifyScripts = new TNotifyScriptParams();
    }

    return NotifyScripts;
}
