/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptStatusCode.h,v 1.3 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004-12-08 00:16:39  thomson
 * DOS end of lines corrected, header added.
 *
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
