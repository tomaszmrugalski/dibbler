/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptOptionRequest.h,v 1.2 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTOPTIONREQUEST_H
#define SRVOPTOPTIONREQUEST_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
#include "OptClientIdentifier.h"
#include "SrvOptClientIdentifier.h"
#include "OptOptionRequest.h"

class TSrvConfMgr;

class TSrvOptOptionRequest : public TOptOptionRequest 
{
  public:
    TSrvOptOptionRequest(char * buf,  int n, TMsg* parent);
    TSrvOptOptionRequest(TMsg* parent);
    bool doDuties();
};

#endif
