/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: DUID.h,v 1.3 2004-03-29 22:06:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 *
 * Released under GNU GPL v2 licence
 *
 */

class TDUID;
#ifndef DUID_H_
#define DUID_H_
#include <iostream>
#include <iomanip>
using namespace std;
class TDUID
{
	friend std::ostream& operator<<(std::ostream& out,TDUID &range);
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
