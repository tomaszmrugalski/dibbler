/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 only licence
 *
 * $Id: SrvOptOptionRequest.cpp,v 1.3 2008-08-29 00:07:37 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2004-10-25 20:45:54  thomson
 * Option support, parsers rewritten. ClntIfaceMgr now handles options.
 *
 */

#include <stdlib.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include <netinet/in.h>
#endif 
#include "SmartPtr.h"
#include "SrvCfgMgr.h"
#include "SrvOptOptionRequest.h"


TSrvOptOptionRequest::TSrvOptOptionRequest( char * buf,  int n, TMsg* parent)
	:TOptOptionRequest(buf,n, parent)
{
}

TSrvOptOptionRequest::TSrvOptOptionRequest(TMsg* parent)
    :TOptOptionRequest(parent)
{

}

bool TSrvOptOptionRequest::doDuties()
{
    return true;
}
