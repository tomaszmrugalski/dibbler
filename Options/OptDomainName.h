#ifndef OPTDOMAINNAME_H
#define OPTDOMAINNAME_H
#include "Opt.h"
#include <iostream>
#include <string>
using namespace std;

class TOptDomainName : public TOpt
{
public:
    TOptDomainName(string DomainName, TMsg* parent);
    TOptDomainName(char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    string getDomainName();
    void setDomainName(string name);

protected:
    string DomainName;
};

#endif
