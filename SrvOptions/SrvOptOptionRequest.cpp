/*
 * Dibbler - a portable DHCPv6
 *
 * authors: Tomasz Mrugalski <thomson@klub.com.pl>
 *          Marek Senderski <msend@o2.pl>
 *
 * released under GNU GPL v2 or later licence
 *
 * $Id: SrvOptOptionRequest.cpp,v 1.2 2004-10-25 20:45:54 thomson Exp $
 *
 * $Log: not supported by cvs2svn $
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
