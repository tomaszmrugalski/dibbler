#ifndef PARSGLOBALOPT_H_
#define PARSGLOBALOPT_H_

#include "ClntParsIfaceOpt.h"

//class TClntParsIfaceOpt;

class TClntParsGlobalOpt : public TClntParsIfaceOpt
{
public:
	TClntParsGlobalOpt();
    
    void setLogLevel(int logLevel);
    int getLogLevel();
    
    void setWorkDir(string dir);
    string getWorkDir();

    void setLogName(string logName);
    string getLogname();

private:
	int LogLevel;
	string LogName;
	string WorkDir;
};

#endif
