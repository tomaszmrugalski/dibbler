/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#include "SrvParsGlobalOpt.h"
#include "Portable.h"
#include "Logger.h"

using namespace std;

TSrvParsGlobalOpt::TSrvParsGlobalOpt(void) {
    WorkDir_          = WORKDIR;
    Stateless_        = false;
    CacheSize_        = SERVER_DEFAULT_CACHE_SIZE;
    Experimental_     = false;
    InterfaceIDOrder_ = SRV_IFACE_ID_ORDER_BEFORE;
    InactiveMode_     = false;
    GuessMode_        = false;
    AuthLifetime_     = DHCPV6_INFINITY;
    AuthKeyLen_       = 16;
    DigestLst_.clear();
}

TSrvParsGlobalOpt::~TSrvParsGlobalOpt(void) {
}

std::string TSrvParsGlobalOpt::getWorkDir() const {
    return WorkDir_;
}

void TSrvParsGlobalOpt::setWorkDir(const std::string& dir) {
    WorkDir_ = dir;
}

void TSrvParsGlobalOpt::setStateless(bool stateless) {
    Stateless_ = stateless;
}

bool TSrvParsGlobalOpt::getStateless() const {
    return Stateless_;
}

void TSrvParsGlobalOpt::setExperimental(bool exper) {
    Experimental_ = exper;
}

bool TSrvParsGlobalOpt::getExperimental() const {
    return Experimental_;
}

void TSrvParsGlobalOpt::setCacheSize(int bytes) {
    CacheSize_ = bytes;
}

int  TSrvParsGlobalOpt::getCacheSize() const {
    return CacheSize_;
}

void TSrvParsGlobalOpt::addDigest(DigestTypes x) {
    SPtr<DigestTypes> dt = new DigestTypes;
    *dt = x;

    DigestLst_.append(dt);
    Log(Debug) << "Auth: New digest type " << getDigestName(x) << " added." << LogEnd;
}

List(DigestTypes) TSrvParsGlobalOpt::getDigest() const {
    return DigestLst_;
}

void TSrvParsGlobalOpt::setInterfaceIDOrder(ESrvIfaceIdOrder order) {
    InterfaceIDOrder_ = order;
}

ESrvIfaceIdOrder TSrvParsGlobalOpt::getInterfaceIDOrder() const {
    return InterfaceIDOrder_;
}

void TSrvParsGlobalOpt::setInactiveMode(bool flex) {
    InactiveMode_ = flex;
}

bool TSrvParsGlobalOpt::getInactiveMode() const {
    return InactiveMode_;
}

void TSrvParsGlobalOpt::setAuthLifetime(unsigned int lifetime) {
    AuthLifetime_ = lifetime;
    Log(Debug) << "AUTH: Authorisation lifetime set to " << lifetime << "." << LogEnd;
}

unsigned int TSrvParsGlobalOpt::getAuthLifetime() const
{
    return AuthLifetime_;
}

void TSrvParsGlobalOpt::setAuthKeyLen(unsigned int len) {
    AuthKeyLen_ = len;
    Log(Debug) << "AUTH: Key gen nonce length set to " << len << "." << LogEnd;
}

unsigned int TSrvParsGlobalOpt::getAuthKeyLen() const {
    return AuthKeyLen_;
}

void TSrvParsGlobalOpt::setGuessMode(bool guess) {
    GuessMode_ = guess;
}
bool TSrvParsGlobalOpt::getGuessMode() const {
    return GuessMode_;
}
