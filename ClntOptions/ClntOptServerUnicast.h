/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptServerUnicast.h,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004-10-25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.2  2004/09/07 17:42:31  thomson
 * Server Unicast implemented.
 *
 */

#ifndef CLNTOPTSERVERUNICAST_H
#define CLNTOPTSERVERUNICAST_H

#include "OptAddr.h"
#include "DHCPConst.h"

class TClntOptServerUnicast : public TOptAddr
{
 public:
    TClntOptServerUnicast(char * buf,  int n, TMsg* parent);
    bool doDuties();
};

#endif /* CLNTOPTSERVERUNICAST_H */
