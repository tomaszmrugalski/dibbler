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
    PeerAddr_ = addr;

    Iface = iface;
    TransID = transID;
    IsDone = false;
    MsgType = msgType;
    DigestType_ = DIGEST_NONE; /* by default digest is none */
    AuthDigestPtr_ = NULL;
    AuthDigestLen_ = 0;
    SPI_ = 0;
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
        Log(Error) << "AUTH: protocol " << auth->getProto() << " not supported yet." << LogEnd;
        return;
    }
    case AUTH_PROTO_DELAYED: {
        if (AuthKey_.empty()) {
            if (MsgType == SOLICIT_MSG) {
                return; // That's alright, no auth info in SOLICIT
            }
            Log(Error) << "AUTH: Can't sign delayed-auth option, key empty." << LogEnd;
            return;
        }
        if (auth->getAuthDataPtr()) {
            hmac_md5(buffer, len, (char*) &AuthKey_[0], AuthKey_.size() ,auth->getAuthDataPtr());
        }
        return;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: {
        if (AuthKey_.size() != RECONFIGURE_KEY_SIZE) {
            Log(Error) << "AUTH: Invalid size of reconfigure-key: expected " <<
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
    return SPtr<TOpt>();
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
    return SPtr<TOpt>();
}

TMsg::~TMsg() {
    if (NotifyScripts) {
        delete NotifyScripts;
    }
}

SPtr<TIPv6Addr> TMsg::getRemoteAddr() {
    return PeerAddr_;
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

bool TMsg::validateAuthInfo(char *buf, int bufSize,
                            AuthProtocols proto,
                            const DigestTypesLst& acceptedDigestTypes) {
    bool is_ok = false;
    bool dt_in_list = false;

    switch (proto) {
    case AUTH_PROTO_NONE:
        return true;
    case AUTH_PROTO_DELAYED: {
        SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
        if (!auth) {
            Log(Warning) << "AUTH: Mandatory AUTH option missing in delayed auth."
                         << LogEnd;
            return false;
        }
        if (auth->getProto() != AUTH_PROTO_DELAYED) {
            Log(Warning) << "AUTH: Bad protocol in auth: expected 2(delayed auth), but got "
                         << int(auth->getProto()) << ", key ignored." << LogEnd;
            return false;
        }
        if (auth->getAlgorithm() != 1) {
            Log(Warning) << "AUTH: Bad algorithm in auth option: expected 1 (HMAC-MD5), but got "
                         << int(auth->getAlgorithm()) << ", key ignored." << LogEnd;
            return false;
        }
        if (MsgType == SOLICIT_MSG) {
            if (auth->getSize() != TOptAuthentication::OPT_AUTH_FIXED_SIZE + TOpt::OPTION6_HDR_LEN) {
                Log(Warning) << "AUTH: Received non-empty delayed-auth option in SOLICIT,"
                             << " expected empty." << LogEnd;
                return false;
            } else {
                return true; // delayed auth in Solicit should come in empty
            }
        }

        if (SPI_ == 0) {
            Log(Warning) << "AUTH: Received invalid SPI = 0." << LogEnd;
            return false;
        }

        if (!loadAuthKey()) {
            Log(Warning) << "AUTH: Failed to load delayed auth key with key id="
                         << std::hex << getSPI() << std::dec << LogEnd;
            return false;
        }

        // Ok, let's do validation
        char *rcvdAuthInfo = new char[RECONFIGURE_DIGEST_SIZE];
        char *goodAuthInfo = new char[RECONFIGURE_DIGEST_SIZE];

        memmove(rcvdAuthInfo, AuthDigestPtr_, DELAYED_AUTH_DIGEST_SIZE);
        memset(AuthDigestPtr_, 0, DELAYED_AUTH_DIGEST_SIZE);

        hmac_md5(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo);

        Log(Debug) << "Auth: Checking delayed-auth (HMAC-MD5) digest:" << LogEnd;
        PrintHex("Auth:received digest: ", (uint8_t*)rcvdAuthInfo, DELAYED_AUTH_DIGEST_SIZE);
        PrintHex("Auth:  proper digest: ", (uint8_t*)goodAuthInfo, DELAYED_AUTH_DIGEST_SIZE);

        if (0 == memcmp(goodAuthInfo, rcvdAuthInfo, DELAYED_AUTH_DIGEST_SIZE))
            is_ok = true;

        delete [] rcvdAuthInfo;
        delete [] goodAuthInfo;

        return is_ok;
    }
    case AUTH_PROTO_RECONFIGURE_KEY: {
        if (MsgType != RECONFIGURE_MSG)
            return true;
        SPtr<TOptAuthentication> auth = (Ptr*)getOption(OPTION_AUTH);
        if (!auth) {
            Log(Warning) << "AUTH: Mandatory AUTH option missing in RECONFIGURE." << LogEnd;
            return false;
        }
        if (auth->getProto() != AUTH_PROTO_RECONFIGURE_KEY) {
            Log(Warning) << "AUTH: Bad protocol in auth: expected 3(reconfigure-key), but got "
                         << int(auth->getProto()) << ", key ignored." << LogEnd;
            return false;
        }
        if (auth->getAlgorithm() != 1) {
            Log(Warning) << "AUTH: Bad algorithm in auth option: expected 1, but got "
                         << int(auth->getAlgorithm()) << ", key ignored." << LogEnd;
            return false;
        }
        if (auth->getRDM() != AUTH_REPLAY_NONE) {
            Log(Warning) << "AUTH: Bad replay detection method (RDM) value: expected 0,"
                         << ", but got " << auth->getRDM() << LogEnd;
            // This is small issue enough, so we can continue.
        }
        if (AuthKey_.size() != RECONFIGURE_KEY_SIZE) {
            Log(Error) << "AUTH: Failed to verify incoming RECONFIGURE message due to "
                       << "reconfigure-key issue: expected size " << RECONFIGURE_KEY_SIZE
                       << ", but got " << AuthKey_.size() << ", message dropped." << LogEnd;
            return false;
        }
        if (!AuthDigestPtr_) {
            Log(Error) << "AUTH: Failed to verify incoming RECONFIGURE message: "
                       << "AuthDigestPtr_ not set, message dropped." << LogEnd;
            return false;
        }

        char *rcvdAuthInfo = new char[RECONFIGURE_DIGEST_SIZE];
        char *goodAuthInfo = new char[RECONFIGURE_DIGEST_SIZE];

        memmove(rcvdAuthInfo, AuthDigestPtr_, RECONFIGURE_DIGEST_SIZE);
        memset(AuthDigestPtr_, 0, RECONFIGURE_DIGEST_SIZE);

        hmac_md5(buf, bufSize, (char*)&AuthKey_[0], AuthKey_.size(), goodAuthInfo);

        Log(Debug) << "Auth: Checking reconfigure-key" << LogEnd;
        PrintHex("Auth:received digest: ", (uint8_t*)rcvdAuthInfo, RECONFIGURE_DIGEST_SIZE);
        PrintHex("Auth:  proper digest: ", (uint8_t*)goodAuthInfo, RECONFIGURE_DIGEST_SIZE);

        if (0 == memcmp(goodAuthInfo, rcvdAuthInfo, RECONFIGURE_DIGEST_SIZE))
            is_ok = true;

        delete [] rcvdAuthInfo;
        delete [] goodAuthInfo;

        return is_ok;
    }
    case AUTH_PROTO_DIBBLER:
        break;
    }
    
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
            Log(Warning) << "Authentication option is required." << LogEnd;
        else
            Log(Warning) << "Authentication method " << getDigestName(DigestType_)
                       << " not accepted." << LogEnd;
        return false;
    }

    if (DigestType_ == DIGEST_NONE) {
            is_ok = true;
    } else if (AuthDigestPtr_) {
#ifndef MOD_DISABLE_AUTH

        if (AuthKey_.empty() && !loadAuthKey()) {
            Log(Debug) << "Auth: Failed to load key with SPI=" << SPI_ << LogEnd;
            return false;
        }

        unsigned AuthInfoLen = getDigestSize(DigestType_);
        char *rcvdAuthInfo = new char[AuthInfoLen];
        char *goodAuthInfo = new char[AuthInfoLen];

        memmove(rcvdAuthInfo, AuthDigestPtr_, AuthInfoLen);
        memset(AuthDigestPtr_, 0, AuthInfoLen);

        switch (DigestType_) {
                case DIGEST_PLAIN:
                    /// @todo: load plain text from a file
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
        return false;
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
		     << " message, but " << authCnt << " received.";
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

void TMsg::setLocalAddr(SPtr<TIPv6Addr> myaddr) {
    LocalAddr_ = myaddr;
}

SPtr<TIPv6Addr> TMsg::getLocalAddr() {
    return LocalAddr_;
}
