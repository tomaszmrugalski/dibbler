/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptElapsed.h,v 1.2 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef SRVOPTELAPSED_H
#define SRVOPTELAPSED_H

#include "OptElapsed.h"

class TSrvOptElapsed : public TOptElapsed
{
 public:
    TSrvOptElapsed(TMsg* parent);
    TSrvOptElapsed( char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif /* SRVOPTELAPSED_H */
