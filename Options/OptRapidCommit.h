#ifndef OPTRAPIDCOMMIT_H
#define OPTRAPIDCOMMIT_H

#include "DHCPConst.h"
#include "Opt.h"

class TOptRapidCommit : public TOpt
{
public:
    TOptRapidCommit(TMsg* parent);
    TOptRapidCommit(char * &buf,  int &n,TMsg* parent);
    int getSize();
    char * storeSelf(char* buf);
};

#endif 
