/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: RelOptRelayMsg.h,v 1.1 2005-01-11 22:53:36 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef RELOPTRELAYMSG_H
#define RELOPTRELAYMSG_H

#include "DHCPConst.h"
#include "OptGeneric.h"

class TRelOptRelayMsg : public TOptGeneric
{
  public:
    TRelOptRelayMsg(char* buf, int bufsize, TMsg* parent);
    bool doDuties();
};

#endif /* RELOPTGENERIC_H */
