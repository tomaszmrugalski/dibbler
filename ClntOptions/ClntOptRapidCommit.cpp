/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: ClntOptRapidCommit.cpp,v 1.2 2004-03-29 18:53:08 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
