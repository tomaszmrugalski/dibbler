#include "ClntParsGlobalOpt.h"
#include "Portable.h"
TClntParsGlobalOpt::TClntParsGlobalOpt() : TClntParsIfaceOpt()
{
    WorkDir  = WORKDIR;
}

void TClntParsGlobalOpt::setWorkDir(string dir) 
{
    this->WorkDir=dir;
}

string TClntParsGlobalOpt::getWorkDir() 
{
    return this->WorkDir;
}
