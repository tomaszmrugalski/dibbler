#ifndef OPTTIMEZONE_H
#define OPTTIMEZONE_H
#include <iostream>
#include <string>

#include "Opt.h"
#include "TimeZone.h"

using namespace std;

class TOptTimeZone : public TOpt
{
public:
    TOptTimeZone(string timeZone, TMsg* parent);
    TOptTimeZone(char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    string getTimeZone();
    void setTimeZone(string timeZone);
    bool isValid();
protected:
    TTimeZone Zone;
};

#endif
