/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: RelOptEcho.h,v 1.1 2008-03-02 20:01:42 thomson Exp $
 *
 */

#ifndef RELOPTECHO_H
#define RELOPTECHO_H

#include "DHCPConst.h"
#include "SmartPtr.h"
#include "OptOptionRequest.h"

class TRelOptEcho;

class TRelOptEcho : public TOptOptionRequest 
{
  public:
    TRelOptEcho(char * buf,  int n, TMsg* parent);
    TRelOptEcho(TMsg* parent);
    bool doDuties();
};

#endif /* RelOptEcho */
