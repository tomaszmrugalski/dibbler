/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptServerUnicast.cpp,v 1.3 2004-10-02 13:11:24 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include "ClntOptServerUnicast.h"

TClntOptServerUnicast::TClntOptServerUnicast( char * buf,  int n, TMsg* parent)
    :TOptServerUnicast(buf,n, parent)
{
}

bool TClntOptServerUnicast::doDuties()
{    
    return false;
}
