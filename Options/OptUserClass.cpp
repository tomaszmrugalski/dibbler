/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 licence
 *
 * $Id: OptUserClass.cpp,v 1.3 2004-09-05 15:27:49 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004/03/29 18:53:08  thomson
 * Author/Licence/cvs log/cvs version headers added.
 *
 *
 */
#include "OptUserClass.h"

TOptUserClass::TOptUserClass( char * &buf,  int &n, TMsg* parent)
	:TOpt(OPTION_USER_CLASS, parent)
{

}

 int TOptUserClass::getSize()
{
    return 0;
}

 char * TOptUserClass::storeSelf( char* buf)
{
    return 0;
}
bool TOptUserClass::isValid()
{
    return true;
}
