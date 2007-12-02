/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ReqOpt.h,v 1.1 2007-12-02 10:32:00 thomson Exp $
 *
 */

#ifndef REQOPT_H
#define REQOPT_H

#include "Opt.h"
#include "OptAddr.h"
#include "OptDUID.h"
#include "OptGeneric.h"

class TReqOptAddr : public TOptAddr
{
public:
    TReqOptAddr(int type, SmartPtr<TIPv6Addr> addr, TMsg * parent);
protected:
    bool doDuties();
};

class TReqOptDUID : public TOptDUID
{
public:
    TReqOptDUID(int type, SmartPtr<TDUID> duid, TMsg* parent);
protected:
    bool doDuties();
};

class TReqOptGeneric : public TOptGeneric
{
public:
    TReqOptGeneric(int optType, char * data, int dataLen, TMsg* parent);
protected:
    bool doDuties();
};

#endif
