/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: ClntOptStatusCode.cpp,v 1.3 2004-06-17 23:53:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include "ClntOptStatusCode.h"

TClntOptStatusCode::TClntOptStatusCode( char * buf,  int len, TMsg* parent)
	:TOptStatusCode(buf,len, parent)
{

}

