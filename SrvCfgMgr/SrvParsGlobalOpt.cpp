/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 */

#include "SrvParsGlobalOpt.h"
#include "Portable.h"

TSrvParsGlobalOpt::TSrvParsGlobalOpt(void) {
    this->WorkDir   = WORKDIR;
    this->Stateless = false;
    this->CacheSize = SERVER_DEFAULT_CACHE_SIZE;
    this->Experimental = false;

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
    this->DigestLst.append(x);
}

List(DigestTypes) TSrvParsGlobalOpt::getDigest() {
    return this->DigestLst;
}
