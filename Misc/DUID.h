/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * $Id: DUID.h,v 1.8 2006-11-03 20:07:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2006-08-21 21:03:44  thomson
 * Load procedure fixed.
 *
 * Revision 1.6  2006/01/12 00:23:35  thomson
 * Cleanup changes. Now -pedantic option works.
 *
 * Revision 1.5  2004/09/28 17:30:47  thomson
 * Header #include added.
 *
 * Revision 1.4  2004/06/20 20:59:30  thomson
 * getPlain() method added.
 *
 * Revision 1.3  2004/03/29 22:06:49  thomson
 * 0.1.1 version
 *
 *
 * Released under GNU GPL v2 licence
 *
 */

class TDUID;
#ifndef DUID_H_
#define DUID_H_
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

class TDUID
{
    friend std::ostream& operator<<(std::ostream& out,TDUID &range);
 public:
    TDUID();
    TDUID(const char* DUID,int DUIDlen); // packed
    TDUID(const char* Plain); // plain
    TDUID(const TDUID &duid);
    TDUID& operator=(const TDUID& duid);
    bool operator==(const TDUID &duid);
    bool operator<=(const TDUID &duid);
    int getLen();
    char * storeSelf(char* buf);
    const string getPlain();
    const char * get();

    ~TDUID();

private:
    void packedToPlain();
    void plainToPacked();
    char* DUID;
    string Plain;
    int	len;	
};

#endif
