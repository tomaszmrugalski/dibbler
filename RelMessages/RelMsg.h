/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelMsg.h,v 1.2 2005-01-11 23:35:22 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/11 22:53:35  thomson
 * Relay skeleton implemented.
 *
 *
 */

class TRelMsg;
#ifndef RELMSG_H
#define RELMSG_H

#include "RelCommon.h"
#include "Msg.h"

class TRelMsg : public TMsg
{
public:
    TRelMsg(TCtx * ctx, int iface,  SmartPtr<TIPv6Addr> addr, char* data,  int dataLen);
    virtual bool check() = 0;
    
 protected:
    TCtx * Ctx;

};

#endif
