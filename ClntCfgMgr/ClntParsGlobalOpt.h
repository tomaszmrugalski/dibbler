#ifndef PARSGLOBALOPT_H_
#define PARSGLOBALOPT_H_

#include "ClntParsIfaceOpt.h"

class TClntParsGlobalOpt : public TClntParsIfaceOpt
{
public:
    TClntParsGlobalOpt();
    
    void setWorkDir(string dir);
    string getWorkDir();


private:
    string WorkDir;
};

#endif
