/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptElapsed.cpp,v 1.3 2004-03-29 19:10:06 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include <stdlib.h>
#ifdef LINUX
#include <netinet/in.h>
#endif
#include <time.h>
#include "DHCPConst.h"
#include "Portable.h"
#include "ClntOptElapsed.h"

TClntOptElapsed::TClntOptElapsed( char * buf,  int n, TMsg* parent)	:TOptElapsed(buf,n, parent){
}
TClntOptElapsed::TClntOptElapsed(TMsg* parent)	:TOptElapsed(parent){}
bool TClntOptElapsed::doDuties(){    return false;}
