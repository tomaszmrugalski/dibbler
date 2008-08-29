/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptRapidCommit.cpp,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005-07-17 19:56:54  thomson
 * End-of-line problem solved.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include "ClntOptRapidCommit.h"


TClntOptRapidCommit::TClntOptRapidCommit( char * buf,  int n, TMsg* parent)
	:TOptRapidCommit(buf,n, parent)
{

}

TClntOptRapidCommit::TClntOptRapidCommit(TMsg* parent)
    :TOptRapidCommit(parent)
{
}

bool TClntOptRapidCommit::doDuties()
{
    return false;
}
