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
    this->WorkDir  = WORKDIR;
    this->Stateless = false;
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

void TSrvParsGlobalOpt::setCacheSize(int bytes) {
    this->CacheSize = bytes;
}

int  TSrvParsGlobalOpt::getCacheSize() {
    return this->CacheSize;
}

