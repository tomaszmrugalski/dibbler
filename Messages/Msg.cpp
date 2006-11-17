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
#include "sha1.h"

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

    if (AuthInfoPtr && getOption(OPTION_AUTH) && DigestType != DIGEST_NONE)
            switch (DigestType) {
                    case DIGEST_HMAC_SHA1:
                        // [s] zmieniæ klucz i jego rozmiar na co¶ sensownego
                        hmac_sha1(start, buffer-start, "testkey", 7, (char *)AuthInfoPtr);
                        cout << "sending digest: ";
                        printhex(AuthInfoPtr, getDigestSize(DigestType));
                        break;
                    default:
                        break;
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

    if (AuthInfoPtr && DigestType != DIGEST_NONE) {
            unsigned AuthInfoLen = getDigestSize(DigestType);
            char *rcvdAuthInfo = new char[AuthInfoLen];
            char *goodAuthInfo = new char[AuthInfoLen];

            switch (DigestType) {
                    case DIGEST_NONE:
                        ok = true;
                        break;
                    case DIGEST_HMAC_SHA1:

                        cout << bufSize << endl;
                        memmove(rcvdAuthInfo, AuthInfoPtr, AuthInfoLen);
                        memset(AuthInfoPtr, 0, AuthInfoLen);
                        // [s] zmieniæ klucz i jego rozmiar na co¶ sensownego
                        hmac_sha1(buf, bufSize, "testkey", 7, goodAuthInfo);
                        cout << "received digest: ";
                        printhex(rcvdAuthInfo, AuthInfoLen);
                        cout << "  proper digest: ";
                        printhex(goodAuthInfo, AuthInfoLen);
                        if (0 == strncmp(goodAuthInfo, rcvdAuthInfo, AuthInfoLen))
                                ok = true;
                        break;
                    default:
                        break;
            }

            delete [] rcvdAuthInfo;
            delete [] goodAuthInfo;
    }

    if (ok)
        Log(Info) << "Authentication Information correct." << LogEnd;
    else
        Log(Error) << "Authentication Information incorrect." << LogEnd;

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
