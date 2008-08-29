/*
 * Dibbler - a portable DHCPv6
 *
 * author: Tomasz Mrugalski <thomson@klub.com.pl>
 * 
 * released under GNU GPL v2 only licence
 *
 */

class TSrvOptLQ;
class TSrvOptLQClientData;
class TSrvOptLQClientTime;
class TSrvOptLQRelayData;
class TSrvOptLQClientLink;

#ifndef SRVOPTLQ_H
#define SRVOPTLQ_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "DUID.h"
#include "IPv6Addr.h"
#include "Opt.h"
#include "OptInteger.h"
#include "OptGeneric.h"


class TSrvOptLQ : public TOpt
{
  public:
    TSrvOptLQ(char * buf, int bufsize, TMsg* parent);    
    bool doDuties();
    ELeaseQueryType getQueryType();
    int getSize();
    char * storeSelf(char* buf);
    SPtr<TIPv6Addr> getLinkAddr();
 private:
    ELeaseQueryType QueryType;
    SPtr<TDUID> Duid;
    SPtr<TIPv6Addr> Addr;

    bool IsValid;
};

class TSrvOptLQClientData : public TOpt
{
public:
    int getSize();
    char* storeSelf(char*);
    bool doDuties();

    TSrvOptLQClientData(TMsg * parent);
    // only suboptions
};

class TSrvOptLQClientTime : public TOptInteger
{
public:
    TSrvOptLQClientTime(unsigned int value, TMsg* parent);
    bool doDuties();
};

// not supported
class TSrvOptLQRelayData : public TOptGeneric
{
public:
    TSrvOptLQRelayData(SPtr<TIPv6Addr> addr, TMsg* parent);
};

class TSrvOptLQClientLink : public TOpt
{
public:
    TSrvOptLQClientLink(List(TIPv6Addr) AddrLst, TMsg * parent);
private:
    List(TIPv6Addr) LinkAddrLst;
};

#endif
