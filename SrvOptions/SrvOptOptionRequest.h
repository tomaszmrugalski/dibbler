/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptOptionRequest.h,v 1.3 2006-02-02 23:17:39 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/10/25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#ifndef SRVOPTOPTIONREQUEST_H
#define SRVOPTOPTIONREQUEST_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "Opt.h"
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
