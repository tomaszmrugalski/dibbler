/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptServerUnicast.h,v 1.2 2004-09-07 17:42:31 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTOPTSERVERUNICAST_H
#define CLNTOPTSERVERUNICAST_H

#include "Opt.h"
#include "DHCPConst.h"
#include "OptServerUnicast.h"

class TClntOptServerUnicast : public TOptServerUnicast 
{
  public:
    TClntOptServerUnicast( char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif /* CLNTOPTSERVERUNICAST_H */
