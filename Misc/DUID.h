class TDUID;
#ifndef DUID_H_
#define DUID_H_
#include <iostream>
#include <iomanip>
using namespace std;
class TDUID
{
    friend ostream& operator<<(ostream& out,TDUID &range);
public:
    TDUID();
    TDUID(char* DUID,int DUIDlen); // packed
    TDUID(char* Plain); // plain
    TDUID(const TDUID &duid);
    TDUID& operator=(const TDUID& duid);
    bool TDUID::operator==(const TDUID &duid);
    bool TDUID::operator<=(const TDUID &duid);
    int getLen();
    char * storeSelf(char* buf);
    ~TDUID();

private:
    char*   DUID;
    int	    len;	
};

#endif
