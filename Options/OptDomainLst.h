/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 */

#ifndef OPTSTRINGLST_H
#define OPTSTRINGLST_H
#include "Opt.h"
#include <iostream>
#include <string>
using namespace std;

class TOptDomainLst : public TOpt
{
public:
    TOptDomainLst(int type, List(string) strLst, TMsg* parent);
    TOptDomainLst(int type, const std::string& domain, TMsg* parent);
    TOptDomainLst(int type, const char *buf, unsigned short bufsize, TMsg* parent);
    const List(string)& getDomainLst() { return StringLst; }
    const std::string& getDomain();
    char * storeSelf( char* buf);
    int getSize();
    int countString();
    bool doDuties() { return true; }
    virtual std::string getPlain();
protected:
    List(string) StringLst;
};

#endif
