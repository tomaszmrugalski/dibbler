/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptStatusCode.h,v 1.2 2004-12-08 00:16:39 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 */

#ifndef CLNTOPTSTATUSCODE_H
#define CLNTOPTSTATUSCODE_H


#include "OptStatusCode.h"
#include "DHCPConst.h"

class TClntOptStatusCode : public TOptStatusCode 
{
  public:
    TClntOptStatusCode( char * buf,  int len, TMsg* parent);
};

#endif /* CLNTOPTSTATUSCODE_H */
