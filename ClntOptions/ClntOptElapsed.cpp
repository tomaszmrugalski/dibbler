/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptElapsed.cpp,v 1.5 2006-01-29 20:02:56 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/07/17 19:56:54  thomson
 * End-of-line problem solved.
 *
 * Revision 1.3  2004/03/29 19:10:06  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */

#include "DHCPConst.h"
#include "ClntOptElapsed.h"

TClntOptElapsed::TClntOptElapsed( char * buf,  int n, TMsg* parent)
    :TOptInteger4(OPTION_ELAPSED_TIME, buf,n, parent)
{
}

TClntOptElapsed::TClntOptElapsed(TMsg* parent)
    :TOptInteger4(OPTION_ELAPSED_TIME, 0, parent){
}
bool TClntOptElapsed::doDuties()
{
    return false;
}
