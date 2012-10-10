/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ReqOpt.h,v 1.3 2008-08-29 00:07:33 thomson Exp $
 *
 */

#ifndef REQOPT_H
#define REQOPT_H

#include "Opt.h"
#include "OptIAAddress.h"
#include "OptDUID.h"
#include "OptGeneric.h"
#include "SmartPtr.h"

class TReqOptAddr : public TOptIAAddress
{
public:
    TReqOptAddr(int type, SPtr<TIPv6Addr> addr, TMsg * parent);

protected:
    bool doDuties();
};

class TReqOptDUID : public TOptDUID
{
public:
    TReqOptDUID(int type, SPtr<TDUID> duid, TMsg* parent);
protected:
    bool doDuties();

};

class TReqOptGeneric : public TOptGeneric
{
public:
    TReqOptGeneric(int optType, char * data, int dataLen, TMsg* parent);
    TReqOptGeneric(int optType, char * data, int dataLen, TMsg* parent, int remoteId);
protected:
    bool doDuties();
};



class TReqOptRemoteId : public TOptGeneric
{
public:
    TReqOptRemoteId(int optType,int remoteId, TMsg* parent);
protected:
    bool doDuties();
};


class TReqOptRelayId : public TOptDUID
{
    public:
        //TReqOptRelayId(int type, SPtr<TDUID> duid, TMsg* parent);
        TReqOptRelayId(int type,int optionLen,SPtr<TDUID> duid,TMsg* parent);
       //int getSize();

        SPtr<TDUID> getRelayDUID();
        char * storeSelf(char *buf);
    protected:
        bool doDuties();
};

#endif
