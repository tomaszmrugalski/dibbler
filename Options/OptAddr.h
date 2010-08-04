/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptAddr.h,v 1.1 2004-10-26 22:36:57 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/09/05 15:27:49  thomson
 * Data receive switched from recvfrom to recvmsg, unicast partially supported.
 *
 *
 */

#ifndef OPTSERVERUNICAST_H
#define OPTSERVERUNICAST_H

#include "Opt.h"
#include "DHCPConst.h"
#include "SmartPtr.h"
#include "IPv6Addr.h"

class TOptAddr : public TOpt
{
 public:
    TOptAddr(int type, const char * buf, unsigned short len, TMsg* parent);
    TOptAddr(int type, SPtr<TIPv6Addr> addr, TMsg * parent);
    int getSize();
    char * storeSelf( char* buf);
    SPtr<TIPv6Addr> getAddr();
    virtual bool doDuties() { return true; } // does nothing on its own
 protected:
    SPtr<TIPv6Addr> Addr;
};

#endif
