#include "SrvParsGlobalOpt.h"
#include "Portable.h"

TSrvParsGlobalOpt::TSrvParsGlobalOpt(void)
{
    LogLevel = 0;
    LogName  = SRVLOG_FILE;
    WorkDir  = WORKDIR;
}

TSrvParsGlobalOpt::~TSrvParsGlobalOpt(void)
{
}

int	TSrvParsGlobalOpt::getLogLevel()
{
    return this->LogLevel;
}

string TSrvParsGlobalOpt::getLogName()
{
    return this->LogName;
}

string TSrvParsGlobalOpt::getWorkDir()
{
    return this->WorkDir;
}

void TSrvParsGlobalOpt::setLogLevel(int logLevel)
{
    this->LogLevel=logLevel;
}

void TSrvParsGlobalOpt::setLogName(string logName)
{
    this->LogName=logName;
}

void TSrvParsGlobalOpt::setWorkDir(string dir)
{
    this->WorkDir=dir;
}
