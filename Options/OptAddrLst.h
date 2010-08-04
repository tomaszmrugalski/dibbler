/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: OptAddrLst.h,v 1.2 2008-08-29 00:07:30 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2004-11-02 01:23:13  thomson
 * Initial revision
 *
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
    int getSize();
    void firstAddr();
    SPtr<TIPv6Addr> getAddr();
    int countAddr();
    bool isValid();
    virtual bool doDuties() { return true; } // does nothing on its own
protected:
    List(TIPv6Addr) AddrLst;
};
#endif
