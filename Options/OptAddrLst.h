/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 */

#ifndef OPTDNSSERVERS_H
#define OPTDNSSERVERS_H
#include "IPv6Addr.h"
#include "Container.h"
#include "SmartPtr.h"
#include "Opt.h"

class TOptAddrLst : public TOpt
{
public:
    TOptAddrLst(int type, List(TIPv6Addr) lst, TMsg* parent);
    TOptAddrLst(int type, const char *buf, unsigned short len, TMsg* parent);
    char * storeSelf( char* buf);
    size_t getSize();
    void firstAddr();
    SPtr<TIPv6Addr> getAddr();
    const List(TIPv6Addr)& getAddrLst() { return AddrLst; }
    int countAddr();
    bool isValid() const;
    virtual bool doDuties() { return true; } // does nothing on its own
    std::string getPlain();
protected:
    List(TIPv6Addr) AddrLst;
};
#endif
