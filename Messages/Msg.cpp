/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 only licence
 */

#include <stdlib.h>
#include <cmath>
#ifdef WIN32
#include <winsock2.h>
#endif
#include "Portable.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Container.h"

#include "Msg.h"
#include "Opt.h"
#include "Logger.h"
#include "hmac.h"

TMsg::TMsg(int iface, SmartPtr<TIPv6Addr> addr, char* &buf, int &bufSize)
{
    setAttribs(iface, addr, 0, 0);
    if (bufSize<4)
	return;
    this->MsgType=buf[0];
    unsigned char * buf2 = (unsigned char *)(buf+1);
    this->TransID= ((long)buf2[0])<<16 | ((long)buf2[1])<<8 | (long)buf2[2];
    buf+=4; bufSize-=4;
    this->pkt = 0;
}

TMsg::TMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType)
{
    long tmp = rand() % (255*255*255);
    setAttribs(iface,addr,msgType,tmp);
    this->pkt = 0;
}

TMsg::TMsg(int iface, SmartPtr<TIPv6Addr> addr, int msgType,  long transID)
{
    setAttribs(iface,addr,msgType,transID);
}

void TMsg::setAttribs(int iface, SmartPtr<TIPv6Addr> addr, int msgType, long transID)
{
    PeerAddr=addr;

    this->Iface=iface;
    TransID=transID;
    IsDone=false;
    MsgType=msgType;
    this->pkt=NULL;
    DigestType = DIGEST_NONE; /* by default digest is none */
    AuthInfoPtr = NULL;
    AuthInfoKey = NULL;
    KeyGenNonce = NULL;
    KeyGenNonceLen = 0;
    AAASPI = 0;
    SPI = 0;
    ReplayDetection = 0;
}

int TMsg::getSize()
{
    SmartPtr<TOpt> Option;
    int pktsize=0;
    Options.first();
    while( Option = Options.get() )
	pktsize+=Option->getSize();
    return pktsize+4;
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

TContainer< SmartPtr<TOpt> > TMsg::getOptLst()
{
    return Options;
}

/* prepares binary version of this message, returns number of bytes used
**/
int TMsg::storeSelf(char * buffer)
{
    char *start = buffer;
    int tmp = this->TransID;

    enum DigestTypes UsedDigestType;
    
    *(buffer++) = (char)MsgType;
    
    /* ugly 3-byte version of htons/htonl */
    buffer[2] = tmp%256;  tmp = tmp/256;
    buffer[1] = tmp%256;  tmp = tmp/256;
    buffer[0] = tmp%256;  tmp = tmp/256;
    buffer+=3;
    Options.first();
    SmartPtr<TOpt> Option;
    while( Option = Options.get() )
    {
        Option->storeSelf(buffer);
        buffer += Option->getSize();
    }

#ifndef MOD_DISABLE_AUTH
    // for AAAAUTH use only HMAC-SHA1
    if (getOption(OPTION_AAAAUTH))
            UsedDigestType = DIGEST_HMAC_SHA1;
    else
            UsedDigestType = DigestType;

    if (AuthInfoKey && AuthInfoPtr && (getOption(OPTION_AUTH) || getOption(OPTION_AAAAUTH)) && UsedDigestType != DIGEST_NONE) {
            Log(Debug) << "Auth: Used digest type is " << getDigestName(UsedDigestType) << LogEnd;
            switch (UsedDigestType) {
                    case DIGEST_PLAIN:
                        memcpy(AuthInfoPtr, "This is 32-byte plain testkey...", getDigestSize(UsedDigestType));
                        break;
                    case DIGEST_HMAC_MD5:
                        hmac_md5(start, buffer-start, AuthInfoKey, AUTHKEYLEN, (char *)AuthInfoPtr);
                        break;
                    case DIGEST_HMAC_SHA1:
                        hmac_sha(start, buffer-start, AuthInfoKey, AUTHKEYLEN, (char *)AuthInfoPtr, 1);
                        break;
                    case DIGEST_HMAC_SHA224:
                        hmac_sha(start, buffer-start, AuthInfoKey, AUTHKEYLEN, (char *)AuthInfoPtr, 224);
                        break;
                    case DIGEST_HMAC_SHA256:
                        hmac_sha(start, buffer-start, AuthInfoKey, AUTHKEYLEN, (char *)AuthInfoPtr, 256);
                        break;
                    case DIGEST_HMAC_SHA384:
                        hmac_sha(start, buffer-start, AuthInfoKey, AUTHKEYLEN, (char *)AuthInfoPtr, 384);
                        break;
                    case DIGEST_HMAC_SHA512:
                        hmac_sha(start, buffer-start, AuthInfoKey, AUTHKEYLEN, (char *)AuthInfoPtr, 512);
                        break;
                    default:
                        break;
            }
            PrintHex("Auth: Sending digest: ", AuthInfoPtr, getDigestSize(UsedDigestType));
    }
#endif

    return buffer-start;
}

SmartPtr<TOpt> TMsg::getOption(int type) {
    SmartPtr<TOpt> Option;
    
    Options.first();
    while ( Option = Options.get() ) {
	if (Option->getOptType()==type) 
	    return Option;
    }
    return SmartPtr<TOpt>();
}

void TMsg::firstOption() {
    Options.first();
}

int TMsg::countOption() {
    return Options.count();
}

SmartPtr<TOpt> TMsg::getOption() {
    return Options.get();
}

TMsg::~TMsg() {
}

SmartPtr<TIPv6Addr> TMsg::getAddr() {
    return PeerAddr;
}

int TMsg::getIface() {
    return this->Iface;
}

bool TMsg::isDone() {
    return IsDone;
}

void TMsg::setAuthInfoPtr(char* ptr) {
    AuthInfoPtr = ptr;
}

void TMsg::setAuthInfoKey(char* ptr) {
    AuthInfoKey = ptr;

    if (!ptr) {
        Log(Debug) << "Strange, NULL pointer to setAuthInfoKey()" << LogEnd;
        return;
    }

    PrintHex("Auth: setting auth info to: ", AuthInfoKey, AUTHKEYLEN);
}

int TMsg::setAuthInfoKey() {
#ifndef MOD_DISABLE_AUTH
    //  key = HMAC-SHA1 (AAA-key, {Key Generation Nonce || client identifier})

    char *KeyGenNonce_ClientID;
    char * AAAkey;
    uint32_t AAAkeyLen;

    if (!KeyGenNonce)
        KeyGenNonceLen = 16;

    // FIXME set proper size of Client ID (DUID?) (here and in hmac_sha())
    KeyGenNonce_ClientID = new char[KeyGenNonceLen+128];

    AAAkey = getAAAKey(AAASPI, &AAAkeyLen);
    string fname = getAAAKeyFilename(AAASPI);

    // error, no file?
    if (!AAAkey) {
        Log(Error) << "Auth: Unable to load key file for SPI " << hex << AAASPI <<": " << fname << " not found." << dec << LogEnd;
        AuthInfoKey = NULL;
        delete KeyGenNonce_ClientID;
        return -1;
    }
    Log(Debug) << "Auth: AAA-key loaded from file " << fname << "." << LogEnd;

    PrintHex("Auth: AAA-key: ", AAAkey, AAAkeyLen);

    memset(KeyGenNonce_ClientID, 0, KeyGenNonceLen+128);
    if (KeyGenNonce)
        memcpy(KeyGenNonce_ClientID, KeyGenNonce, KeyGenNonceLen);

    // FIXME fill also with ClientID (DUID?)

    PrintHex("Auth: Infokey: using KeyGenNonce+CliendID: ", KeyGenNonce_ClientID, KeyGenNonceLen+128);

    Log(Debug) << "Auth: AAAKeyLen: " << AAAkeyLen << ", KeyGenNonceLen: " << KeyGenNonceLen << LogEnd;
    AuthInfoKey = new char[AUTHKEYLEN];
    hmac_sha(KeyGenNonce_ClientID, KeyGenNonceLen+128, AAAkey, AAAkeyLen, (char *)AuthInfoKey, 1);

    PrintHex("Auth: AuthInfoKey (calculated): ", AuthInfoKey, AUTHKEYLEN);

    delete KeyGenNonce_ClientID;
#endif

    return 0;
}

char * TMsg::getAuthInfoKey() {
    return AuthInfoKey;
}

void TMsg::setAAASPI(uint32_t val) {
    AAASPI = val;
}

uint32_t TMsg::getAAASPI() {
    return AAASPI;
}

void TMsg::setSPI(uint32_t val) {
    SPI = val;
}

uint32_t TMsg::getSPI() {
    return SPI;
}

void TMsg::setReplayDetection( uint64_t value)
{
	ReplayDetection = value;
}

uint64_t TMsg::getReplayDetection() {
    return ReplayDetection;
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

bool TMsg::validateAuthInfo(char *buf, int bufSize) {
    List(DigestTypes) lst;
    lst.clear();

    return validateAuthInfo(buf, bufSize, lst);
}

bool TMsg::validateAuthInfo(char *buf, int bufSize, List(DigestTypes) authLst) {
    bool is_ok = false;
    SmartPtr<DigestTypes> dt;
    bool dt_in_list = false;
    
    //empty list means that any digest type is accepted
    if (authLst.count() == 0) {
        dt_in_list = true;
    } else {
        authLst.first();
        while (dt = authLst.get()) {
            if (*dt == DigestType) {
                dt_in_list = true;
                break;
            }
        }
    }

    if (dt_in_list == false) {
        if (DigestType == DIGEST_NONE)
            Log(Error) << "Authentication option is required." << LogEnd;
        else
            Log(Error) << "Authentication method " << getDigestName(DigestType) << " not accepted." << LogEnd;
        return false;
    }

    if (DigestType == DIGEST_NONE) {
            is_ok = true;
    } else if (AuthInfoPtr) {
#ifndef MOD_DISABLE_AUTH
        unsigned AuthInfoLen = getDigestSize(DigestType);
        char *rcvdAuthInfo = new char[AuthInfoLen];
        char *goodAuthInfo = new char[AuthInfoLen];

        if (!AuthInfoKey) {
            Log(Debug) << "Auth: No AuthInfoKey was set. This could mean bad SPI or no AAA-SPI file." << LogEnd;
            return false;
        }

        memmove(rcvdAuthInfo, AuthInfoPtr, AuthInfoLen);
        memset(AuthInfoPtr, 0, AuthInfoLen);

        switch (DigestType) {
                // [s] change the key to something that make sense
                case DIGEST_PLAIN:
                    memcpy(goodAuthInfo, "This is 32-byte plain testkey...", 32); break;
                case DIGEST_HMAC_MD5:
                    hmac_md5(buf, bufSize, AuthInfoKey, AUTHKEYLEN, goodAuthInfo); break;
                case DIGEST_HMAC_SHA1:
                    hmac_sha(buf, bufSize, AuthInfoKey, AUTHKEYLEN, goodAuthInfo, 1); break;
                case DIGEST_HMAC_SHA224:
                    hmac_sha(buf, bufSize, AuthInfoKey, AUTHKEYLEN, goodAuthInfo, 224); break;
                case DIGEST_HMAC_SHA256:
                    hmac_sha(buf, bufSize, AuthInfoKey, AUTHKEYLEN, goodAuthInfo, 256); break;
                case DIGEST_HMAC_SHA384:
                    hmac_sha(buf, bufSize, AuthInfoKey, AUTHKEYLEN, goodAuthInfo, 384); break;
                case DIGEST_HMAC_SHA512:
                    hmac_sha(buf, bufSize, AuthInfoKey, AUTHKEYLEN, goodAuthInfo, 512); break;
                default:
                    break;
        }
        if (0 == memcmp(goodAuthInfo, rcvdAuthInfo, AuthInfoLen))
            is_ok = true;

        Log(Debug) << "Auth:Checking using digest method: " << DigestType << LogEnd;
        PrintHex("Auth:received digest: ", rcvdAuthInfo, AuthInfoLen);
        PrintHex("Auth:  proper digest: ", goodAuthInfo, AuthInfoLen);

        delete [] rcvdAuthInfo;
        delete [] goodAuthInfo;

        if (is_ok)
            Log(Info) << "Authentication Information correct." << LogEnd;
        else
            Log(Error) << "Authentication Information incorrect." << LogEnd;
#endif
    } else {
      Log(Error) << "Auth: Digest mode set to " << DigestType << ", but AUTH option not set." << LogEnd;
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
    SmartPtr<TOpt> option;
    int clntCnt=0;
    int srvCnt =0;
    int authCnt = 0;
    bool status = true;

    Options.first();
    while (option = Options.get() ) 
    {
	switch (option->getOptType()) {
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
    }

    if (authCnt > 1) {
	Log(Warning) << "No more that one authentication option is allowed in the " << this->getName()
		     << " message, but " << srvCnt << " received.";
    }

    if (!status)
	Log(Cont) << "Message dropped." << LogEnd;

    return status;
}
