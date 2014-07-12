/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 */

#ifndef CLNTOPTOPTIONREQUEST_H
#define CLNTOPTOPTIONREQUEST_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "OptOptionRequest.h"

class TClntConfMgr;

class TClntOptOptionRequest : public TOptOptionRequest 
{
 public:
    TClntOptOptionRequest(SPtr<TClntCfgIface> ptrIface, TMsg* parent);
    TClntOptOptionRequest( char * buf,  int n, TMsg* parent);
    bool doDuties();
 private:
    SPtr<TClntCfgMgr> CfgMgr;
};

#endif
