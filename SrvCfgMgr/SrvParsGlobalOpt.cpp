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
