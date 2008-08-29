/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: ClntOptPreference.cpp,v 1.4 2008-08-29 00:07:29 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2004-10-25 20:45:53  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include "DHCPConst.h"
#include "ClntOptPreference.h"

TClntOptPreference::TClntOptPreference( char * buf,  int n, TMsg* parent)
	:TOptPreference(buf,n, parent)
{

}

TClntOptPreference::TClntOptPreference( char pref, TMsg* parent)
	:TOptPreference(pref, parent)
{
}

bool TClntOptPreference::doDuties()
{
    return false;
}
