#include "SrvParsGlobalOpt.h"
#include "Portable.h"

TSrvParsGlobalOpt::TSrvParsGlobalOpt(void)
{
    WorkDir  = WORKDIR;
}

TSrvParsGlobalOpt::~TSrvParsGlobalOpt(void)
{
}

string TSrvParsGlobalOpt::getWorkDir()
{
    return this->WorkDir;
}

void TSrvParsGlobalOpt::setWorkDir(string dir)
{
    this->WorkDir=dir;
}
