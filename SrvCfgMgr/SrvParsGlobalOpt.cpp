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

TSrvParsGlobalOpt::TSrvParsGlobalOpt(void) {
    this->WorkDir          = WORKDIR;
    this->Stateless        = false;
    this->CacheSize        = SERVER_DEFAULT_CACHE_SIZE;
    this->Experimental     = false;
    this->InterfaceIDOrder = SRV_IFACE_ID_ORDER_BEFORE;
    this->InactiveMode     = false;
    this->GuessMode        = false;
    this->AuthLifetime     = DHCPV6_INFINITY;
    this->AuthKeyLen       = 16;

    this->DigestLst.clear();
}

TSrvParsGlobalOpt::~TSrvParsGlobalOpt(void) {
}

string TSrvParsGlobalOpt::getWorkDir() {
    return this->WorkDir;
}

void TSrvParsGlobalOpt::setWorkDir(string dir) {
    this->WorkDir=dir;
}

void TSrvParsGlobalOpt::setStateless(bool stateless) {
    this->Stateless = stateless;
}

bool TSrvParsGlobalOpt::getStateless() {
    return this->Stateless;
}

void TSrvParsGlobalOpt::setExperimental(bool exper) {
    this->Experimental = exper;
}

bool TSrvParsGlobalOpt::getExperimental() {
    return this->Experimental;
}

void TSrvParsGlobalOpt::setCacheSize(int bytes) {
    this->CacheSize = bytes;
}

int  TSrvParsGlobalOpt::getCacheSize() {
    return this->CacheSize;
}

void TSrvParsGlobalOpt::addDigest(DigestTypes x) {
    SPtr<DigestTypes> dt = new DigestTypes;
    *dt = x;

    this->DigestLst.append(dt);
    Log(Debug) << "Auth: New digest type " << getDigestName(x) << " added." << LogEnd;
}

List(DigestTypes) TSrvParsGlobalOpt::getDigest() {
    return this->DigestLst;
}

void TSrvParsGlobalOpt::setInterfaceIDOrder(ESrvIfaceIdOrder order) {
    InterfaceIDOrder = order;
}

ESrvIfaceIdOrder TSrvParsGlobalOpt::getInterfaceIDOrder() {
    return InterfaceIDOrder;
}

void TSrvParsGlobalOpt::setInactiveMode(bool flex)
{
    InactiveMode = flex;
}

bool TSrvParsGlobalOpt::getInactiveMode()
{
    return InactiveMode;
}

void TSrvParsGlobalOpt::setAuthLifetime(unsigned int lifetime)
{
    AuthLifetime = lifetime;
    Log(Debug) << "AUTH: Authorisation lifetime set to " << lifetime << "." << LogEnd;
}

unsigned int TSrvParsGlobalOpt::getAuthLifetime()
{
    return AuthLifetime;
}

void TSrvParsGlobalOpt::setAuthKeyLen(unsigned int len)
{
    AuthKeyLen = len;
    Log(Debug) << "AUTH: Key gen nonce length set to " << len << "." << LogEnd;
}

unsigned int TSrvParsGlobalOpt::getAuthKeyLen()
{
    return AuthKeyLen;
}

void TSrvParsGlobalOpt::setGuessMode(bool guess)
{
    GuessMode = guess;
}
bool TSrvParsGlobalOpt::getGuessMode()
{
    return GuessMode;
}
