/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: OptAddrLst.h,v 1.1 2004-11-02 01:23:13 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
    TOptAddrLst(int type, char *&buf, int &bufsize, TMsg* parent);
    char * storeSelf( char* buf);
    int getSize();
    void firstAddr();
    SmartPtr<TIPv6Addr> getAddr();
    int countAddr();
    bool isValid();
protected:
    List(TIPv6Addr) AddrLst;
    bool Valid;

};
#endif
