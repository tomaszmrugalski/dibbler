#include "ClntParsGlobalOpt.h"
#include "Portable.h"
TClntParsGlobalOpt::TClntParsGlobalOpt() : TClntParsIfaceOpt()
{
	LogLevel = 8;
	LogName  = "Client";
	WorkDir  = WORKDIR;
}

void TClntParsGlobalOpt::setLogLevel(int logLevel) 
{
    this->LogLevel=logLevel;
}

int TClntParsGlobalOpt::getLogLevel() 
{ 
    return this->LogLevel;
}

void TClntParsGlobalOpt::setWorkDir(string dir) 
{
    this->WorkDir=dir;
}

string TClntParsGlobalOpt::getWorkDir() 
{
    return this->WorkDir;
}

void TClntParsGlobalOpt::setLogName(string logName) 
{ 
    this->LogName=logName;
}

string TClntParsGlobalOpt::getLogname() 
{
    return this->LogName;
}
