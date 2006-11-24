/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 * changes: Michal Kowalczuk <michal@kowalczuk.eu>
 *
 * released under GNU GPL v2 or later licence
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
    char *key = (char *)"testkey";
    char *start = buffer;
    int tmp = this->TransID;
    
    //pkt_start = start;
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

    if (AuthInfoPtr && getOption(OPTION_AUTH) && DigestType != DIGEST_NONE) {
            // [s] change the key to something that make sense
            switch (DigestType) {
                    case DIGEST_PLAIN:
                        memcpy(AuthInfoPtr, "This is 32-byte plain testkey...", getDigestSize(DigestType));
                        break;
                    case DIGEST_HMAC_MD5:
                        hmac_md5(start, buffer-start, key, 7, (char *)AuthInfoPtr);
                        break;
                    case DIGEST_HMAC_SHA1:
                        hmac_sha(start, buffer-start, key, 7, (char *)AuthInfoPtr, 1);
                        break;
                    case DIGEST_HMAC_SHA224:
                        hmac_sha(start, buffer-start, key, 7, (char *)AuthInfoPtr, 224);
                        break;
                    case DIGEST_HMAC_SHA256:
                        hmac_sha(start, buffer-start, key, 7, (char *)AuthInfoPtr, 256);
                        break;
                    case DIGEST_HMAC_SHA384:
                        hmac_sha(start, buffer-start, key, 7, (char *)AuthInfoPtr, 384);
                        break;
                    case DIGEST_HMAC_SHA512:
                        hmac_sha(start, buffer-start, key, 7, (char *)AuthInfoPtr, 512);
                        break;
                    default:
                        break;
            }
            cout << "#### sending digest: ";
            printhex(AuthInfoPtr, getDigestSize(DigestType));
    }

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

bool TMsg::validateAuthInfo(char *buf, int bufSize) {
    bool ok = false;
    char *key = (char *)"testkey";

    if (DigestType == DIGEST_NONE) {
            ok = true;
            Log(Debug) << "Authentication is disabled." << LogEnd;
    } else if (AuthInfoPtr) {
            unsigned AuthInfoLen = getDigestSize(DigestType);
            char *rcvdAuthInfo = new char[AuthInfoLen];
            char *goodAuthInfo = new char[AuthInfoLen];

            cout << "#### " << bufSize << endl;
            memmove(rcvdAuthInfo, AuthInfoPtr, AuthInfoLen);
            memset(AuthInfoPtr, 0, AuthInfoLen);

            switch (DigestType) {
                    // [s] change the key to something that make sense
                    case DIGEST_PLAIN:
                        memcpy(goodAuthInfo, "This is 32-byte plain testkey...", 32); break;
                    case DIGEST_HMAC_MD5:
                        hmac_md5(buf, bufSize, key, 7, goodAuthInfo); break;
                    case DIGEST_HMAC_SHA1:
                        hmac_sha(buf, bufSize, key, 7, goodAuthInfo, 1); break;
                    case DIGEST_HMAC_SHA224:
                        hmac_sha(buf, bufSize, key, 7, goodAuthInfo, 224); break;
                    case DIGEST_HMAC_SHA256:
                        hmac_sha(buf, bufSize, key, 7, goodAuthInfo, 256); break;
                    case DIGEST_HMAC_SHA384:
                        hmac_sha(buf, bufSize, key, 7, goodAuthInfo, 384); break;
                    case DIGEST_HMAC_SHA512:
                        hmac_sha(buf, bufSize, key, 7, goodAuthInfo, 512); break;
                    default:
                        break;
            }
            if (0 == strncmp(goodAuthInfo, rcvdAuthInfo, AuthInfoLen))
                ok = true;

            cout << "#### received digest: ";
            printhex(rcvdAuthInfo, AuthInfoLen);
            cout << "####   proper digest: ";
            printhex(goodAuthInfo, AuthInfoLen);

            delete [] rcvdAuthInfo;
            delete [] goodAuthInfo;

            if (ok)
                Log(Info) << "Authentication Information correct." << LogEnd;
            else
                Log(Error) << "Authentication Information incorrect." << LogEnd;

    }
    
    return ok;
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

    if (!status)
	Log(Cont) << "Message dropped." << LogEnd;

    return status;
}
